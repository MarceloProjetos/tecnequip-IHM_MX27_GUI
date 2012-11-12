#include "defines.h"
#include "GtkUtils.h"

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>

// Modos de operacao
#define MAQ_MODO_INATIVO       0x0000
#define MAQ_AQUEC_MODO_AUTO    0x0100
#define MAQ_ESGUICHO_MODO_AUTO 0x0200
#define MAQ_ESGUICHO_CENTRAL   0x0400

// Estados da maquina, liga/desliga dispositivos
#define MAQ_ESTADO_BOMBA     0x0800
#define MAQ_ESTADO_AQUEC     0x1000
#define MAQ_ESTADO_ESGUICHO1 0x2000
#define MAQ_ESTADO_ESGUICHO2 0x4000
#define MAQ_ESTADO_ESGUICHO3 0x8000

void MaqConfigAquec(uint16_t modo)
{
  uint16_t modo_atual = MaqLerFlags();

  if(modo == MAQ_AQUEC_MODO_AUTO) {
    modo_atual |= MAQ_AQUEC_MODO_AUTO;
  } else {
    modo_atual &= ~MAQ_AQUEC_MODO_AUTO;
  }

  MaqConfigFlags(modo_atual);
}

void MaqConfigEsguicho(uint16_t modo)
{
  uint16_t modo_atual = MaqLerFlags();

  if(modo == MAQ_ESGUICHO_MODO_AUTO) {
    modo_atual |= MAQ_ESGUICHO_MODO_AUTO;
  } else {
    modo_atual &= ~MAQ_ESGUICHO_MODO_AUTO;
  }

  MaqConfigFlags(modo_atual);
}

void MaqConfigEsguichoCentral(uint16_t estado)
{
  uint16_t modo_atual = MaqLerFlags();

  if(estado) {
    modo_atual |=  MAQ_ESGUICHO_CENTRAL;
  } else {
    modo_atual &= ~MAQ_ESGUICHO_CENTRAL;
  }

  MaqConfigFlags(modo_atual);
}

// Hora para Ligar / Desligar
#define BANHO_START  5
#define BANHO_STOP  22

// 12 posicoes, 1 para cada mes.
unsigned int scheduler_table   [12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
// Tabela para feriados
unsigned int scheduler_table_hl[12] = {
    0x00000001, // Janeiro   - dia 1
    0x00000000, // Fevereiro - sem feriados
    0x00000000, // Marco     - sem feriados
    0x00100000, // Abril     - dia 21
    0x00000001, // Maio      - dia 1
    0x00000400, // Junho     - dia 11
    0x00000000, // Julho     - sem feriados
    0x00000000, // Agosto    - sem feriados
    0x00000040, // Setembro  - dia 7
    0x00000800, // Outubro   - dia 12
    0x00004002, // Novembro  - dias 2 e 15
    0x01000000, // Dezembro  - dia 25
};

#define SCHED_INACTIVE    0
#define SCHED_NORMAL      1
#define SCHED_ANTECIPATED 2

#define SCHED_ARQ_CONFIG "scheduler.backup"
#define SCHED_ARQ_MAGIC  0x872B5A0F

// Função que retorna checksum de ponteiro
extern unsigned long CalcCheckSum(void *ptr, unsigned int tam);

// Funcao que grava o agendamento no disco
void scheduler_backup(void)
{
  long fd = open(SCHED_ARQ_CONFIG, O_WRONLY | O_CREAT, 0666);
  unsigned long checksum, magico = SCHED_ARQ_MAGIC;

  if(fd<0)
    return;

// Grava o número mágico para identificação do arquivo
  write(fd, &magico, sizeof(magico));

// Grava os dados
  write(fd, scheduler_table, sizeof(scheduler_table));

// Calcula e grava o checksum dos dados
  checksum = CalcCheckSum((void *)(scheduler_table), sizeof(scheduler_table));
  write(fd, &checksum, sizeof(checksum));

  close(fd);
}

// Funcao que le o agendamento do disco
int scheduler_restore(void)
{
  unsigned long scheduler_tmp[12];
  long fd = open(SCHED_ARQ_CONFIG, O_RDONLY), ret=0;
  unsigned long val;

  if(fd<0)
    return ret;

  read(fd, &val, sizeof(val));
  if(val == SCHED_ARQ_MAGIC) {
    read(fd, scheduler_tmp, sizeof(scheduler_tmp));

    read(fd, &val, sizeof(val));
    if(val == CalcCheckSum((void *)(scheduler_tmp), sizeof(scheduler_tmp))) {
      memcpy(scheduler_table, scheduler_tmp, sizeof(scheduler_tmp));
    }
  }

  close(fd);

  return ret;
}

unsigned int scheduler_is_valid(GDate *date)
{
  GDate *now = g_date_new();
  unsigned int now_day, now_month, now_year, day, month, year;

  // Carrega mes e ano atuais
  g_date_set_time_t(now, time(NULL));
  now_day   = g_date_get_day  (now);
  now_month = g_date_get_month(now);
  now_year  = g_date_get_year (now);
  g_date_free(now);

  // Carrega mes e ano solicitados
  day   = g_date_get_day  (date);
  month = g_date_get_month(date);
  year  = g_date_get_year (date);

  // Checa se data solicitada esta no scheduler
  if( (((month == now_month && day >= now_day) || month > now_month) // Ano atual
      && year == now_year) || // Ano atual
      (month <  now_month && year == now_year+1)) { // Ano seguinte
    return 1;
  }

  return 0;
}

unsigned int scheduler_is_active(GDate *date)
{
  GDate *last;
  unsigned int day, month;

  if(scheduler_is_valid(date)) {
    // Carrega dia e mes solicitados
    // Remove 1 de cada para ficar no range de 0...n ao inves de 1...n+1
    day   = g_date_get_day  (date) - 1;
    month = g_date_get_month(date) - 1;

    if((scheduler_table[month] >> day) & 1) {
      // Carrega dia anterior
      last = g_date_new_dmy(day+1, month+1, g_date_get_year(date));
      g_date_subtract_days(last, 1);
      // Remove 1 de cada para ficar no range de 0...n ao inves de 1...n+1
      day   = g_date_get_day  (last) - 1;
      month = g_date_get_month(last) - 1;
      // Libera objeto de data
      g_date_free(last);

      if((scheduler_table[month] >> day) & 1)
        return SCHED_NORMAL;
      else
        return SCHED_ANTECIPATED;
    }
  }

  return SCHED_INACTIVE;
}

void scheduler_set_active(GDate *date, unsigned int state)
{
  unsigned int day, month;

  if(scheduler_is_valid(date)) {
    // Carrega dia e mes solicitados
    // Remove 1 de cada para ficar no range de 0...n ao inves de 1...n+1
    day   = g_date_get_day  (date) - 1;
    month = g_date_get_month(date) - 1;
    if(state)
      scheduler_table[month] |=   1UL << day;
    else
      scheduler_table[month] &= ~(1UL << day);
  }
}

// Desmarca como preenchido o mes solicitado
void scheduler_invalidate_month(GDate *date)
{
  scheduler_table[g_date_get_month(date)-1] &= ~(1 << 31);
}

void scheduler_fill_month(GDate *date)
{
  unsigned int day, month, weekday;

  // Remove 1 de mes para ficar no range de 0...11 ao inves de 1...12
  month = g_date_get_month(date) - 1;

  if(scheduler_is_valid(date) && !(scheduler_table[month] & (1UL << 31))) {
    // Marca mes como preenchido, limpando o restante
    scheduler_table[month] = 1UL << 31;

    // Avanca para o primeiro dia do proximo mes
    g_date_add_months   (date, 1);
    g_date_set_day      (date, 1);

    do {
       g_date_subtract_days(date, 1); // Retorna dia a dia para preencher tabela
       day     = g_date_get_day    (date) - 1;
       weekday = g_date_get_weekday(date);

       if(weekday != G_DATE_SATURDAY && weekday != G_DATE_SUNDAY)
         scheduler_table[month] |= 1UL << day;
     } while(day>0); // Se chegou no primeiro dia, terminou o preenchimento

    scheduler_table[month] &= ~scheduler_table_hl[month];

    scheduler_backup();
  }
}

unsigned int AquecModoAuto()
{
  GtkToggleButton *tgb = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "ckbBanhoAquecAuto" ));

  return gtk_toggle_button_get_active(tgb);
}

void DesligaComandosManuais()
{
  uint32_t i;
  GtkToggleButton *tgb;
  char *tgb_list[] = {
      "btnBanhoManualEsguicho1",
      "btnBanhoManualEsguicho2",
      "btnBanhoManualEsguicho3",
      "btnBanhoManualBomba",
      "btnBanhoManualAquec",
      "",
  };

  for(i=0; tgb_list[i][0]; i++) {
    tgb = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, tgb_list[i]));
    gtk_toggle_button_set_active(tgb, 0);
  }
}

gboolean tmrScheduler(gpointer data)
{
  struct tm *now;
  uint16_t maq_auto;
  time_t t = time(NULL);
  GDate *date = g_date_new();
  unsigned int modo_auto, state;
  unsigned int start = BANHO_START, stop = BANHO_STOP; // Horario p/ ligar/desligar equipamento

  // Carrega data atual
  g_date_set_time_t(date, t);
  now = localtime(&t);

  modo_auto = AquecModoAuto(); // Aquecimento configurado para modo automatico
  maq_auto  = (MaqLerFlags() & MAQ_AQUEC_MODO_AUTO); // Modo automatico do aquecimento ativado no CLP

  state = scheduler_is_active(date);
  if(state == SCHED_ANTECIPATED)
    start--;

  // Identifica inicio de um novo mes
  if(now->tm_hour == 0 && now->tm_mday == 1 && GTK_IS_CALENDAR(data)) {
    gtk_calendar_select_month(GTK_CALENDAR(data),
        g_date_get_month(date)-1, g_date_get_year(date));
    g_date_subtract_months(date, 1);
    scheduler_invalidate_month(date);
  }

  // Checa agendamento
  if       (modo_auto && maq_auto && now->tm_hour >= stop) {
    MaqConfigAquec(MAQ_MODO_INATIVO);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "ckbBanhoEsguichoAuto")), 0);
  } else if(modo_auto && !maq_auto && now->tm_hour >= start &&
      now->tm_hour < stop && state != SCHED_INACTIVE) {
    MaqConfigAquec(MAQ_AQUEC_MODO_AUTO);
  }

  g_date_free(date);

  return TRUE;
}

gchar * CalendarDetails(GtkCalendar *calendar, guint year, guint month, guint day, gpointer user_data)
{
  unsigned int start = BANHO_START;
  GDate *date = g_date_new_dmy(day, month+1, year);
  gchar *pstr = NULL;

  switch(scheduler_is_active(date)) {
  case SCHED_ANTECIPATED:
      start--;
      /* no break */
  case SCHED_NORMAL:
    pstr = (gchar *)malloc(50);
    sprintf(pstr, "Ligar %02d:00", start);
    break;
  }

  g_date_free(date);

  return pstr;
}

void CalendarUpdateDetails(GtkCalendar *calendar, GDate *date)
{
  unsigned int month, year;

  month = g_date_get_month(date) - 1;
  year  = g_date_get_year (date);
  g_date_subtract_months(date, 1);

  // Muda o calendario para o mes anterior e retorna para que os detalhes de todos
  // os dias sejam atualizados
  gtk_calendar_select_month(calendar, g_date_get_month(date) - 1, g_date_get_year (date));
  gtk_calendar_select_month(calendar, month, year);
}

void cbCalendarMonthChanged(GtkCalendar *calendar, gpointer user_data)
{
  GDate *date;
  unsigned int d, m, y;

  atividade++;

  gtk_calendar_get_date(calendar, &y, &m, &d);
  date = g_date_new_dmy(1, m+1, y);
  g_date_add_months   (date, 1); // Seleciona 1 mes depois do atual
  g_date_subtract_days(date, 1); // Regressa 1 dia para ficar no ultimo dia do mes

  scheduler_fill_month(date);

  g_date_free(date);
}

void cbCalendarDaySelected(GtkCalendar *calendar, gpointer user_data)
{
  GDate *date;
  unsigned int d, m, y;

  atividade++;

  gtk_calendar_get_date(calendar, &y, &m, &d);
  if(!d) // Dia invalido,MaqConfigModo provavelmente dia desmarcado
    return;

  date = g_date_new_dmy(d, m+1, y);

  scheduler_set_active(date, scheduler_is_active(date) ? 0 : 1);
  CalendarUpdateDetails(calendar, date);

  gtk_calendar_select_day(calendar, 0);

  scheduler_backup();

  g_date_free(date);
}

void cbAtivaAquec(GtkToggleButton *togglebutton, gpointer user_data)
{
  atividade++;

  if(gtk_toggle_button_get_active(togglebutton)) {
    // Chama agendador para entrar no modo automatico se estiver ativo
    tmrScheduler(NULL);
  } else {
    MaqConfigAquec(MAQ_MODO_INATIVO);
  }
}

void cbAtivaEsguicho(GtkToggleButton *togglebutton, gpointer user_data)
{
  atividade++;

  if(gtk_toggle_button_get_active(togglebutton)) {
    MaqConfigEsguicho(MAQ_ESGUICHO_MODO_AUTO);
  } else {
    MaqConfigEsguicho(MAQ_MODO_INATIVO);
  }
}

void cbUsarEsguichoCentral(GtkToggleButton *togglebutton, gpointer user_data)
{
  atividade++;

  MaqConfigEsguichoCentral(gtk_toggle_button_get_active(togglebutton));
}

void cbModoAlterado(GtkToggleButton *togglebutton, gpointer user_data)
{
  uint32_t i, ModoAuto;
  GtkWidget *wdg = GTK_WIDGET(gtk_builder_get_object(builder, "ntbBanhoComandos"));
  GtkToggleButton *tgb;
  char *tgb_list[] = {
      "ckbBanhoAquecAuto",
      "ckbBanhoEsguichoAuto",
      "btnBanhoManualEsguicho1",
      "btnBanhoManualEsguicho2",
      "btnBanhoManualEsguicho3",
      "btnBanhoManualBomba",
      "btnBanhoManualAquec",
      "",
  };

  atividade++;

  for(i=0; tgb_list[i][0]; i++) {
    tgb = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, tgb_list[i]));
    gtk_toggle_button_set_active(tgb, 0);
  }

  ModoAuto = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "rdbBanhoModoAuto" )));
  gtk_notebook_set_current_page(GTK_NOTEBOOK(wdg), ModoAuto);
  MaqConfigModo(ModoAuto ? MAQ_MODO_AUTO : MAQ_MODO_MANUAL);
}

void HabilitaEsguichoManual(uint32_t ativo)
{
  uint32_t i;
  GtkWidget *wdg;
  char *wdg_list[] = {
      "btnBanhoManualEsguicho1",
      "btnBanhoManualEsguicho2",
      "btnBanhoManualEsguicho3",
      "",
  };

  for(i=0; wdg_list[i][0]; i++) {
    wdg = GTK_WIDGET(gtk_builder_get_object(builder, wdg_list[i]));
    gtk_widget_set_sensitive(wdg, ativo);
  }
}

gboolean tmrHabilitaEsguichoManual(gpointer data)
{
  HabilitaEsguichoManual(1);
  return FALSE;
}

void cbManualEsguicho(GtkToggleButton *togglebutton, gpointer user_data)
{
  gchar    *nome;
  uint16_t  estado = MaqLerFlags();
  uint16_t  esguicho;

  nome = (gchar *)gtk_widget_get_name(GTK_WIDGET(togglebutton));
  switch(atoi(&nome[strlen(nome)-1])) {
  case 1:
    esguicho = MAQ_ESTADO_ESGUICHO1;
    break;

  case 2:
    esguicho = MAQ_ESTADO_ESGUICHO2;
    break;

  case 3:
    esguicho = MAQ_ESTADO_ESGUICHO3;
    break;

  default: // Invalido
    return;
  }

  if(gtk_toggle_button_get_active(togglebutton)) {
    estado |=  esguicho;
  } else {
    estado &= ~esguicho;
  }

  MaqConfigFlags(estado);

  // Desativa comando de esguichos enquanto liga um deles.
  HabilitaEsguichoManual(0);
  g_timeout_add_seconds(20, tmrHabilitaEsguichoManual, NULL);
}

void cbManualBomba(GtkToggleButton *togglebutton, gpointer user_data)
{
  uint16_t estado = MaqLerFlags();

  if(gtk_toggle_button_get_active(togglebutton)) {
    estado |=  MAQ_ESTADO_BOMBA;
  } else {
    estado &= ~MAQ_ESTADO_BOMBA;
  }

  MaqConfigFlags(estado);
}

void cbManualAquec(GtkToggleButton *togglebutton, gpointer user_data)
{
  uint16_t estado = MaqLerFlags();

  if(gtk_toggle_button_get_active(togglebutton)) {
    estado |=  MAQ_ESTADO_AQUEC;
  } else {
    estado &= ~MAQ_ESTADO_AQUEC;
  }

  MaqConfigFlags(estado);
}

void Banho_Erro(int erro)
{
  GtkToggleButton *tgb;

  if(!erro) { // Saiu de estado de erro
    // Atualiza o modo atual
    cbModoAlterado(NULL, NULL);
    // Ativa o aquecimento se estiver em modo automatico
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "rdbBanhoModoAuto" )))) {
      tgb = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "ckbBanhoAquecAuto" ));
      gtk_toggle_button_set_active(tgb, 1);
      tmrScheduler(NULL);
    }
  }
}

int Banho_Init(void)
{
  GtkToggleButton *tgb;
  GtkCalendar *calendar;
  GDate *date = g_date_new();

  calendar = GTK_CALENDAR(gtk_builder_get_object(builder, "cldBanhoData"));

  // Carrega lista do disco
  scheduler_restore();

  g_date_set_time_t(date, time(NULL));
  gtk_calendar_set_detail_func(calendar, CalendarDetails, NULL, NULL);
  gtk_calendar_select_month(calendar, g_date_get_month(date)-1, g_date_get_year(date));
  gtk_calendar_select_day  (calendar, 0);
  cbCalendarMonthChanged(calendar, NULL); // Preenche o mes atual

  // Chama funcao que identifica o modo atual, ativando-o.
  // Inicialmente o modo selecionado eh sempre o automatico.
  cbModoAlterado(NULL, NULL);

  // Ativa o aquecimento para o modo automatico
  tgb = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "ckbBanhoAquecAuto" ));
  gtk_toggle_button_set_active(tgb, 1);

  // Iniciando os timers
  g_timeout_add_seconds( 600, tmrScheduler, (gpointer)calendar);

  // Executa diretamente a primeira vez ou a maquina vai checar se
  // precisa ativar o modo automatico somente depois de 5 minutos
  tmrScheduler(NULL);

  g_date_free(date);

  return 1;
}
