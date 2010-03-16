-- MySQL dump 10.13  Distrib 5.1.37, for debian-linux-gnu (i486)
--
-- Host: 192.168.0.2    Database: cv
-- ------------------------------------------------------
-- Server version	5.0.32-Debian_7etch5-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Not dumping tablespaces as no INFORMATION_SCHEMA.FILES table on this server
--

--
-- Table structure for table `clientes`
--

DROP TABLE IF EXISTS `clientes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `clientes` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `nome` varchar(20) NOT NULL,
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `nome` (`nome`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Cadastro de Clientes';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `clientes`
--

LOCK TABLES `clientes` WRITE;
/*!40000 ALTER TABLE `clientes` DISABLE KEYS */;
INSERT INTO `clientes` VALUES (8,'Altamira'),(7,'C4'),(11,'kmokmo'),(10,'Marcelo Miranda'),(1,'Nenhum'),(9,'Outro Cliente'),(2,'Tecnequip');
/*!40000 ALTER TABLE `clientes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `lista_permissoes`
--

DROP TABLE IF EXISTS `lista_permissoes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `lista_permissoes` (
  `ID` int(11) unsigned NOT NULL auto_increment COMMENT 'ID da permissão',
  `nome` varchar(20) character set latin1 collate latin1_general_cs NOT NULL COMMENT 'Nome da permissão',
  `descricao` varchar(20) character set latin1 collate latin1_general_cs default NULL COMMENT 'Descrição da permissão',
  `valor` int(10) unsigned NOT NULL default '0' COMMENT 'Valor padrão desta permissão',
  `tipo` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Lista de permissões';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `lista_permissoes`
--

LOCK TABLES `lista_permissoes` WRITE;
/*!40000 ALTER TABLE `lista_permissoes` DISABLE KEYS */;
INSERT INTO `lista_permissoes` VALUES (5,'acesso_config','Acessar Configuracao',0,1),(6,'acesso_manut','Acessar Manutencao',0,1),(7,'acesso_oper_auto','Acessar Operacao',0,1),(8,'acesso_logs','Acessar Informacoes',0,1);
/*!40000 ALTER TABLE `lista_permissoes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `log`
--

DROP TABLE IF EXISTS `log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `Data` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  `ID_Usuario` int(10) unsigned NOT NULL,
  `Evento` text NOT NULL,
  `Tipo` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  KEY `FK_Usuario` (`ID_Usuario`),
  CONSTRAINT `FK_Usuario` FOREIGN KEY (`ID_Usuario`) REFERENCES `usuarios` (`ID`) ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Registro de Eventos';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `log`
--

LOCK TABLES `log` WRITE;
/*!40000 ALTER TABLE `log` DISABLE KEYS */;
INSERT INTO `log` VALUES (2699,'2008-07-15 19:36:30',1,'Entrada no sistema',1),(2700,'2008-07-15 19:36:32',1,'Erro: Emergencia',3),(2701,'2008-07-15 19:37:24',1,'Inicializando mÃ¡quina',2),(2702,'2008-07-15 19:37:35',1,'Saida do sistema',1),(2703,'2008-07-15 19:39:11',1,'Entrada no sistema',1),(2704,'2008-07-15 19:39:13',1,'Erro: Emergencia',3),(2705,'2008-07-15 19:39:45',1,'Entrada no sistema',1),(2706,'2008-07-15 19:39:47',1,'Erro: Emergencia',3),(2707,'2009-11-24 18:35:41',1,'Erro durante login',3),(2708,'2009-11-24 18:35:50',7,'Erro durante login',3),(2709,'2009-11-24 18:48:13',1,'Saida do sistema',1),(2710,'2009-11-24 18:50:57',1,'Saida do sistema',1),(2711,'2009-11-24 18:52:23',1,'Saida do sistema',1),(2712,'2009-11-24 18:52:35',1,'Saida do sistema',1),(2713,'2009-11-24 18:52:48',1,'Saida do sistema',1),(2714,'2009-11-25 09:20:33',1,'Saida do sistema',1),(2715,'2009-11-25 09:20:44',1,'Saida do sistema',1),(2716,'2009-11-25 09:35:44',1,'Saida do sistema',1),(2717,'2009-11-25 10:17:02',1,'Saida do sistema',1),(2718,'2009-11-25 10:27:53',1,'Saida do sistema',1),(2719,'2009-11-25 11:28:27',1,'Saida do sistema',1),(2720,'2009-11-25 11:41:42',1,'Saida do sistema',1),(2721,'2009-11-25 11:51:01',1,'Saida do sistema',1),(2722,'2009-11-25 11:57:36',1,'Saida do sistema',1),(2723,'2009-11-25 12:02:52',1,'Saida do sistema',1),(2724,'2009-11-25 12:03:47',1,'Saida do sistema',1),(2725,'2009-11-25 12:24:38',1,'Saida do sistema',1),(2726,'2009-11-25 13:38:33',1,'Saida do sistema',1),(2727,'2009-11-25 13:39:40',1,'Saida do sistema',1),(2728,'2009-11-25 15:34:36',1,'Saida do sistema',1),(2729,'2009-11-25 15:54:08',1,'Saida do sistema',1),(2730,'2009-11-25 15:57:05',1,'Saida do sistema',1),(2731,'2009-11-25 16:06:01',1,'Saida do sistema',1),(2732,'2009-11-25 16:08:40',1,'Saida do sistema',1),(2733,'2009-11-25 16:08:44',8,'Erro durante login',3),(2734,'2009-11-25 16:09:05',8,'Saida do sistema',1),(2735,'2009-11-25 16:10:15',1,'Saida do sistema',1),(2736,'2009-11-25 16:10:32',8,'Saida do sistema',1),(2737,'2009-11-25 16:38:13',1,'Removendo usuÃ¡rio x',1),(2738,'2009-11-25 16:38:31',1,'Saida do sistema',1),(2739,'2009-11-25 16:39:05',1,'Adicionado o usuÃ¡rio d',1),(2740,'2009-11-25 16:39:15',1,'Removendo usuÃ¡rio d',1),(2741,'2009-11-25 16:39:54',1,'Adicionado o usuÃ¡rio 4',1),(2742,'2009-11-25 16:40:26',1,'Removendo usuÃ¡rio 4',1),(2743,'2009-11-25 16:43:37',1,'Adicionado o usuÃ¡rio a',1),(2744,'2009-11-25 16:43:41',1,'Saida do sistema',1),(2745,'2009-11-25 16:44:06',1,'Removendo usuÃ¡rio a',1),(2746,'2009-11-25 17:26:27',1,'Saida do sistema',1),(2747,'2009-11-25 17:30:17',1,'Adicionado o usuÃ¡rio r',1),(2748,'2009-11-25 17:30:29',1,'Removendo usuÃ¡rio r',1),(2749,'2009-11-25 17:37:03',1,'Saida do sistema',1),(2750,'2009-11-25 17:38:59',1,'Adicionado o usuÃ¡rio 4',1),(2751,'2009-11-25 17:39:19',1,'Removendo usuÃ¡rio 4',1),(2752,'2009-11-25 17:40:35',1,'Saida do sistema',1),(2753,'2009-11-25 17:42:46',1,'Adicionado o usuÃ¡rio v',1),(2754,'2009-11-25 17:43:01',1,'Removendo usuÃ¡rio v',1),(2755,'2009-11-25 17:49:50',1,'Saida do sistema',1),(2756,'2009-11-25 17:51:25',1,'Adicionado o usuÃ¡rio l',1),(2757,'2009-11-25 17:51:36',1,'Removendo usuÃ¡rio l',1),(2758,'2009-11-25 17:57:08',1,'Adicionado o usuÃ¡rio 1',1),(2759,'2009-11-25 17:57:21',1,'Removendo usuÃ¡rio 1',1),(2760,'2009-11-25 17:59:16',1,'Saida do sistema',1),(2761,'2009-11-25 18:25:50',1,'Saida do sistema',1),(2762,'2009-11-26 11:42:45',1,'Saida do sistema',1),(2763,'2009-11-26 11:46:00',1,'Saida do sistema',1),(2764,'2009-11-26 11:48:32',1,'Saida do sistema',1),(2765,'2009-11-26 15:18:33',1,'Saida do sistema',1),(2766,'2009-11-26 15:42:34',1,'Saida do sistema',1),(2767,'2009-11-26 16:26:35',1,'Saida do sistema',1),(2768,'2009-11-26 16:30:26',1,'Alterado o usuÃ¡rio utikawa',1),(2769,'2009-11-26 16:37:01',1,'Saida do sistema',1),(2770,'2009-11-26 16:43:53',1,'Saida do sistema',1),(2771,'2009-11-26 16:47:28',1,'Saida do sistema',1),(2772,'2009-11-27 09:43:28',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2773,'2009-11-27 15:31:11',1,'Saida do sistema',1),(2774,'2009-11-27 15:59:08',1,'Saida do sistema',1),(2775,'2009-11-27 16:35:40',1,'Saida do sistema',1),(2776,'2009-11-27 16:50:36',1,'Saida do sistema',1),(2777,'2009-11-27 17:28:43',1,'Saida do sistema',1),(2778,'2009-11-27 18:52:45',1,'Saida do sistema',1),(2779,'2009-11-27 19:10:54',1,'Saida do sistema',1),(2780,'2009-11-30 09:41:05',1,'Saida do sistema',1),(2781,'2009-11-30 09:46:33',1,'Saida do sistema',1),(2782,'2009-11-30 09:56:17',1,'Saida do sistema',1),(2783,'2009-11-30 09:59:17',1,'Saida do sistema',1),(2784,'2009-11-30 10:21:00',1,'Saida do sistema',1),(2785,'2009-11-30 10:33:18',1,'Saida do sistema',1),(2786,'2009-11-30 10:34:25',1,'Saida do sistema',1),(2787,'2009-11-30 10:42:05',1,'Saida do sistema',1),(2788,'2009-11-30 10:48:09',1,'Saida do sistema',1),(2789,'2009-11-30 11:23:57',1,'Saida do sistema',1),(2790,'2009-11-30 11:26:51',1,'Saida do sistema',1),(2791,'2009-11-30 12:44:30',1,'Saida do sistema',1),(2792,'2009-12-01 09:28:02',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2793,'2009-12-08 11:57:33',1,'Entrada no sistema',1),(2794,'2009-12-08 11:58:31',1,'Saida do sistema',1),(2795,'2009-12-08 12:02:49',1,'Entrada no sistema',1),(2796,'2009-12-08 12:03:01',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2797,'2010-02-01 10:29:22',1,'Entrada no sistema',1),(2798,'2010-02-01 10:31:04',1,'Saida do sistema',1),(2799,'2010-02-01 10:31:21',1,'Entrada no sistema',1),(2800,'2010-02-01 10:31:37',1,'Saida do sistema',1),(2801,'2010-02-01 10:31:49',1,'Entrada no sistema',1),(2802,'2010-02-01 10:33:01',1,'Saida do sistema',1),(2803,'2010-02-01 12:03:29',1,'Entrada no sistema',1),(2804,'2010-02-01 12:04:21',1,'Saida do sistema',1),(2805,'2010-02-01 12:23:03',1,'Entrada no sistema',1),(2806,'2010-02-01 12:23:47',1,'Saida do sistema',1),(2807,'2010-02-01 12:50:27',1,'Entrada no sistema',1),(2808,'2010-02-01 12:54:49',1,'Saida do sistema',1),(2809,'2010-02-01 13:13:44',1,'Entrada no sistema',1),(2810,'2010-02-01 13:22:26',1,'Saida do sistema',1),(2811,'2010-02-01 13:22:30',1,'Entrada no sistema',1),(2812,'2010-02-01 13:22:42',1,'Saida do sistema',1),(2813,'2010-02-01 13:37:23',1,'Entrada no sistema',1),(2814,'2010-02-01 13:37:40',1,'Saida do sistema',1),(2815,'2010-02-01 13:37:51',1,'Entrada no sistema',1),(2816,'2010-02-01 13:38:06',1,'Saida do sistema',1),(2817,'2010-02-01 13:44:44',1,'Entrada no sistema',1),(2818,'2010-02-01 13:45:00',1,'Saida do sistema',1),(2819,'2010-02-01 15:44:13',1,'Entrada no sistema',1),(2820,'2010-02-01 15:44:35',1,'Saida do sistema',1),(2821,'2010-02-01 16:31:24',1,'Entrada no sistema',1),(2822,'2010-02-01 16:36:44',1,'Entrada no sistema',1),(2823,'2010-02-01 16:47:36',1,'Entrada no sistema',1),(2824,'2010-02-01 16:47:57',1,'Saida do sistema',1),(2825,'2010-02-01 16:48:23',1,'Entrada no sistema',1),(2826,'2010-02-01 17:31:35',1,'Entrada no sistema',1),(2827,'2010-02-01 17:31:53',1,'Saida do sistema',1),(2828,'2010-02-01 17:37:56',1,'Entrada no sistema',1),(2829,'2010-02-01 17:38:22',1,'Saida do sistema',1),(2830,'2010-02-01 17:39:41',1,'Entrada no sistema',1),(2831,'2010-02-01 17:40:26',1,'Saida do sistema',1),(2832,'2010-02-01 17:51:47',1,'Entrada no sistema',1),(2833,'2010-02-01 17:52:40',1,'Saida do sistema',1),(2834,'2010-02-01 17:56:22',1,'Entrada no sistema',1),(2835,'2010-02-01 17:57:03',1,'Saida do sistema',1),(2836,'2010-02-01 17:57:19',1,'Entrada no sistema',1),(2837,'2010-02-01 17:59:02',1,'Saida do sistema',1),(2838,'2010-02-01 18:04:04',1,'Entrada no sistema',1),(2839,'2010-02-01 18:04:43',1,'Saida do sistema',1),(2840,'2010-02-01 18:08:26',1,'Entrada no sistema',1),(2841,'2010-02-01 18:08:34',1,'Saida do sistema',1),(2842,'2010-02-01 18:08:42',1,'Entrada no sistema',1),(2843,'2010-02-01 18:10:30',1,'Saida do sistema',1),(2844,'2010-02-01 18:32:36',1,'Entrada no sistema',1),(2845,'2010-02-01 18:33:59',1,'Saida do sistema',1),(2846,'2010-02-01 18:34:10',1,'Entrada no sistema',1),(2847,'2010-02-01 18:35:40',1,'Saida do sistema',1),(2848,'2010-02-01 18:47:16',1,'Entrada no sistema',1),(2849,'2010-02-01 19:47:24',1,'Saida do sistema',1),(2850,'2010-02-02 10:41:41',1,'Entrada no sistema',1),(2851,'2010-02-02 10:42:00',1,'Saida do sistema',1),(2852,'2010-02-02 10:50:56',1,'Entrada no sistema',1),(2853,'2010-02-02 11:03:25',1,'Saida do sistema',1),(2854,'2010-02-02 11:06:08',1,'Entrada no sistema',1),(2855,'2010-02-02 11:23:57',1,'Saida do sistema',1),(2856,'2010-02-02 11:40:27',1,'Entrada no sistema',1),(2857,'2010-02-02 11:40:59',1,'Saida do sistema',1),(2858,'2010-02-02 11:54:58',1,'Entrada no sistema',1),(2859,'2010-02-02 11:55:18',1,'Saida do sistema',1),(2860,'2010-02-02 13:03:20',1,'Entrada no sistema',1),(2861,'2010-02-02 13:03:49',1,'Saida do sistema',1),(2862,'2010-02-02 13:04:10',1,'Entrada no sistema',1),(2863,'2010-02-02 13:04:31',1,'Saida do sistema',1),(2864,'2010-02-02 13:08:38',1,'Entrada no sistema',1),(2865,'2010-02-02 13:08:55',1,'Saida do sistema',1),(2866,'2010-02-02 13:48:29',1,'Entrada no sistema',1),(2867,'2010-02-02 13:48:50',1,'Saida do sistema',1),(2868,'2010-02-02 13:49:16',1,'Entrada no sistema',1),(2869,'2010-02-02 13:49:39',1,'Saida do sistema',1),(2870,'2010-02-02 13:50:18',1,'Entrada no sistema',1),(2871,'2010-02-02 15:24:27',1,'Entrada no sistema',1),(2872,'2010-02-02 15:24:52',1,'Saida do sistema',1),(2873,'2010-02-02 15:25:11',1,'Entrada no sistema',1),(2874,'2010-02-02 15:25:24',1,'Saida do sistema',1),(2875,'2010-02-02 15:29:45',1,'Entrada no sistema',1),(2876,'2010-02-02 15:30:04',1,'Saida do sistema',1),(2877,'2010-02-02 15:32:59',1,'Entrada no sistema',1),(2878,'2010-02-02 15:33:24',1,'Saida do sistema',1),(2879,'2010-02-02 15:33:50',1,'Entrada no sistema',1),(2880,'2010-02-02 15:34:10',1,'Saida do sistema',1),(2881,'2010-02-02 15:35:27',1,'Entrada no sistema',1),(2882,'2010-02-02 15:35:57',1,'Saida do sistema',1),(2883,'2010-02-02 15:37:05',1,'Entrada no sistema',1),(2884,'2010-02-02 16:21:41',1,'Saida do sistema',1),(2885,'2010-02-02 18:38:14',1,'Entrada no sistema',1),(2886,'2010-02-02 18:40:13',1,'Saida do sistema',1),(2887,'2010-02-03 10:46:06',1,'Entrada no sistema',1),(2888,'2010-02-03 10:46:12',1,'Saida do sistema',1),(2889,'2010-02-03 18:11:33',1,'Entrada no sistema',1),(2890,'2010-02-03 18:11:53',1,'Saida do sistema',1),(2891,'2010-02-03 18:12:50',1,'Entrada no sistema',1),(2892,'2010-02-03 18:13:04',1,'Saida do sistema',1),(2893,'2010-02-05 13:36:52',1,'Entrada no sistema',1),(2894,'2010-02-05 13:37:44',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2895,'2010-02-05 13:37:54',1,'Saida do sistema',1),(2896,'2010-02-05 13:39:15',1,'Entrada no sistema',1),(2897,'2010-02-05 13:39:47',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2898,'2010-02-05 13:39:52',1,'Saida do sistema',1),(2899,'2010-02-05 18:30:36',1,'Entrada no sistema',1),(2900,'2010-02-05 18:31:31',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2901,'2010-02-05 18:31:48',1,'Saida do sistema',1),(2902,'2010-02-08 09:18:05',1,'Entrada no sistema',1),(2903,'2010-02-08 09:18:26',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2904,'2010-02-08 09:18:38',1,'Saida do sistema',1),(2905,'2010-02-08 09:24:15',1,'Entrada no sistema',1),(2906,'2010-02-08 09:24:28',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2907,'2010-02-08 09:29:21',1,'Saida do sistema',1),(2908,'2010-02-08 09:34:31',1,'Entrada no sistema',1),(2909,'2010-02-08 09:34:49',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2910,'2010-02-08 09:35:30',1,'Saida do sistema',1),(2911,'2010-02-08 09:37:59',1,'Entrada no sistema',1),(2912,'2010-02-08 09:38:15',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2913,'2010-02-08 10:07:42',1,'Saida do sistema',1),(2914,'2010-02-08 10:07:59',1,'Entrada no sistema',1),(2915,'2010-02-08 10:08:35',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2916,'2010-02-08 10:09:05',1,'Saida do sistema',1),(2917,'2010-02-08 10:12:00',1,'Entrada no sistema',1),(2918,'2010-02-08 10:12:24',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2919,'2010-02-08 10:12:34',1,'Saida do sistema',1),(2920,'2010-02-08 10:13:48',1,'Entrada no sistema',1),(2921,'2010-02-08 10:14:01',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2922,'2010-02-08 10:14:06',1,'Saida do sistema',1),(2923,'2010-02-08 10:48:31',1,'Entrada no sistema',1),(2924,'2010-02-08 10:48:46',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2925,'2010-02-08 10:49:11',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2926,'2010-02-08 10:49:13',1,'Saida do sistema',1),(2927,'2010-02-08 10:49:22',1,'Entrada no sistema',1),(2928,'2010-02-08 10:49:38',1,'Saida do sistema',1),(2929,'2010-02-08 11:19:23',1,'Entrada no sistema',1),(2930,'2010-02-08 11:20:31',1,'Entrada no sistema',1),(2931,'2010-02-08 11:20:38',1,'Saida do sistema',1),(2932,'2010-02-08 11:21:15',1,'Entrada no sistema',1),(2933,'2010-02-08 11:21:26',1,'Saida do sistema',1),(2934,'2010-02-08 11:23:50',1,'Entrada no sistema',1),(2935,'2010-02-08 11:24:00',1,'Saida do sistema',1),(2936,'2010-02-08 11:34:44',1,'Entrada no sistema',1),(2937,'2010-02-08 11:35:13',1,'Saida do sistema',1),(2938,'2010-02-08 11:39:10',1,'Entrada no sistema',1),(2939,'2010-02-08 11:39:31',1,'Saida do sistema',1),(2940,'2010-02-08 11:58:27',1,'Entrada no sistema',1),(2941,'2010-02-08 12:08:02',1,'Entrada no sistema',1),(2942,'2010-02-08 12:08:36',1,'Saida do sistema',1),(2943,'2010-02-08 12:13:20',1,'Entrada no sistema',1),(2944,'2010-02-08 12:13:38',1,'Saida do sistema',1),(2945,'2010-02-08 12:13:58',1,'Entrada no sistema',1),(2946,'2010-02-08 12:14:06',1,'Saida do sistema',1),(2947,'2010-02-08 12:16:01',1,'Entrada no sistema',1),(2948,'2010-02-08 12:16:15',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2949,'2010-02-08 12:16:46',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2950,'2010-02-08 12:16:52',1,'Saida do sistema',1),(2951,'2010-02-08 12:18:02',1,'Entrada no sistema',1),(2952,'2010-02-08 12:18:23',1,'Saida do sistema',1),(2953,'2010-02-08 12:24:50',1,'Entrada no sistema',1),(2954,'2010-02-08 12:25:00',1,'Saida do sistema',1),(2955,'2010-02-08 12:30:39',1,'Entrada no sistema',1),(2956,'2010-02-08 12:30:55',1,'Saida do sistema',1),(2957,'2010-02-08 12:32:19',1,'Entrada no sistema',1),(2958,'2010-02-08 12:32:37',1,'Saida do sistema',1),(2959,'2010-02-08 12:33:03',1,'Entrada no sistema',1),(2960,'2010-02-08 12:33:12',1,'Saida do sistema',1),(2961,'2010-02-08 13:19:33',1,'Entrada no sistema',1),(2962,'2010-02-08 13:19:43',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2963,'2010-02-08 13:19:46',1,'Saida do sistema',1),(2964,'2010-02-08 13:21:04',1,'Entrada no sistema',1),(2965,'2010-02-08 13:21:14',1,'Saida do sistema',1),(2966,'2010-02-08 18:32:38',1,'Entrada no sistema',1),(2967,'2010-02-08 18:32:50',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2968,'2010-02-08 18:32:52',1,'Saida do sistema',1),(2969,'2010-02-08 19:27:29',1,'Entrada no sistema',1),(2970,'2010-02-08 19:27:51',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2971,'2010-02-08 19:27:57',1,'Saida do sistema',1),(2972,'2010-02-08 19:32:53',1,'Entrada no sistema',1),(2973,'2010-02-08 19:33:02',1,'Saida do sistema',1),(2974,'2010-02-09 09:26:32',1,'Entrada no sistema',1),(2975,'2010-02-09 09:26:35',1,'Saida do sistema',1),(2976,'2010-02-09 09:29:54',1,'Entrada no sistema',1),(2977,'2010-02-09 09:30:02',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2978,'2010-02-09 09:30:09',1,'Saida do sistema',1),(2979,'2010-02-09 09:30:15',1,'Entrada no sistema',1),(2980,'2010-02-09 09:30:20',1,'Saida do sistema',1),(2981,'2010-02-09 10:02:43',1,'Entrada no sistema',1),(2982,'2010-02-09 10:03:16',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2983,'2010-02-09 10:03:24',1,'Saida do sistema',1),(2984,'2010-02-09 10:03:34',1,'Entrada no sistema',1),(2985,'2010-02-09 10:03:48',1,'Saida do sistema',1),(2986,'2010-02-09 10:04:38',1,'Entrada no sistema',1),(2987,'2010-02-09 10:04:46',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2988,'2010-02-09 10:04:48',1,'Saida do sistema',1),(2989,'2010-02-09 10:04:53',1,'Entrada no sistema',1),(2990,'2010-02-09 10:17:04',1,'Saida do sistema',1),(2991,'2010-02-09 16:27:45',1,'Entrada no sistema',1),(2992,'2010-02-09 16:31:02',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2993,'2010-02-09 16:31:10',1,'Saida do sistema',1),(2994,'2010-02-09 16:34:07',1,'Entrada no sistema',1),(2995,'2010-02-09 16:34:51',1,'Saida do sistema',1),(2996,'2010-02-09 16:38:40',1,'Entrada no sistema',1),(2997,'2010-02-09 16:38:53',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(2998,'2010-02-09 16:38:55',1,'Saida do sistema',1),(2999,'2010-02-09 16:41:53',1,'Entrada no sistema',1),(3000,'2010-02-09 16:42:21',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(3001,'2010-02-09 16:42:58',1,'Saida do sistema',1),(3002,'2010-02-09 16:43:14',1,'Entrada no sistema',1),(3003,'2010-02-09 16:43:24',1,'Saida do sistema',1),(3004,'2010-02-09 17:10:34',1,'Entrada no sistema',1),(3005,'2010-02-09 17:10:42',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(3006,'2010-02-09 17:10:47',1,'Saida do sistema',1),(3007,'2010-02-09 17:44:28',1,'Entrada no sistema',1),(3008,'2010-02-09 17:44:54',1,'Saida do sistema',1),(3009,'2010-02-09 17:47:58',1,'Entrada no sistema',1),(3010,'2010-02-09 17:48:11',1,'Saida do sistema',1),(3011,'2010-02-09 17:48:45',1,'Entrada no sistema',1),(3012,'2010-02-09 17:48:57',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(3013,'2010-02-09 17:50:00',1,'Saida do sistema',1),(3014,'2010-02-11 16:29:17',1,'Entrada no sistema',1),(3015,'2010-02-11 16:29:26',1,'Saida do sistema',1),(3016,'2010-02-11 17:20:07',1,'Entrada no sistema',1),(3017,'2010-02-11 17:21:58',1,'Saida do sistema',1),(3018,'2010-02-11 17:35:44',1,'Entrada no sistema',1),(3019,'2010-02-11 17:35:52',1,'Saida do sistema',1),(3020,'2010-02-11 17:37:03',1,'Entrada no sistema',1),(3021,'2010-02-11 17:37:11',1,'Saida do sistema',1),(3022,'2010-02-11 17:38:18',1,'Entrada no sistema',1),(3023,'2010-02-11 17:38:58',1,'Saida do sistema',1),(3024,'2010-02-11 17:40:21',1,'Entrada no sistema',1),(3025,'2010-02-11 17:40:39',1,'Saida do sistema',1),(3026,'2010-02-11 17:43:59',1,'Entrada no sistema',1),(3027,'2010-02-11 17:45:16',1,'Saida do sistema',1),(3028,'2010-02-12 09:47:18',1,'Entrada no sistema',1),(3029,'2010-02-12 09:49:26',1,'Saida do sistema',1),(3030,'2010-02-12 09:56:26',1,'Entrada no sistema',1),(3031,'2010-02-12 09:56:39',1,'Saida do sistema',1),(3032,'2010-02-12 09:59:41',1,'Entrada no sistema',1),(3033,'2010-02-12 09:59:49',1,'Saida do sistema',1),(3034,'2010-02-12 10:00:34',1,'Entrada no sistema',1),(3035,'2010-02-12 10:00:45',1,'Saida do sistema',1),(3036,'2010-02-12 10:01:21',1,'Entrada no sistema',1),(3037,'2010-02-12 10:04:16',1,'Saida do sistema',1),(3038,'2010-02-12 10:39:48',1,'Entrada no sistema',1),(3039,'2010-02-12 10:40:17',1,'Saida do sistema',1),(3040,'2010-02-12 10:40:34',1,'Entrada no sistema',1),(3041,'2010-02-12 10:40:48',1,'Saida do sistema',1),(3042,'2010-02-12 10:42:17',1,'Entrada no sistema',1),(3043,'2010-02-12 10:42:24',1,'Saida do sistema',1),(3044,'2010-02-12 10:44:55',1,'Entrada no sistema',1),(3045,'2010-02-12 10:45:12',1,'Saida do sistema',1),(3046,'2010-02-12 10:48:10',1,'Entrada no sistema',1),(3047,'2010-02-12 10:48:30',1,'Saida do sistema',1),(3048,'2010-02-12 10:49:03',1,'Entrada no sistema',1),(3049,'2010-02-12 10:49:11',1,'Saida do sistema',1),(3050,'2010-02-12 10:49:41',1,'Entrada no sistema',1),(3051,'2010-02-12 10:50:07',1,'Saida do sistema',1),(3052,'2010-02-12 10:50:28',1,'Entrada no sistema',1),(3053,'2010-02-12 11:06:01',1,'Saida do sistema',1),(3054,'2010-02-12 11:16:44',1,'Entrada no sistema',1),(3055,'2010-02-12 11:17:15',1,'Saida do sistema',1),(3056,'2010-03-04 13:39:08',1,'Entrada no sistema',1),(3057,'2010-03-04 13:39:23',1,'Saida do sistema',1),(3058,'2010-03-04 13:41:15',1,'Entrada no sistema',1),(3059,'2010-03-04 13:41:31',1,'Saida do sistema',1),(3060,'2010-03-04 13:42:36',1,'Entrada no sistema',1),(3061,'2010-03-04 13:42:50',1,'Saida do sistema',1),(3062,'2010-03-04 13:44:54',1,'Entrada no sistema',1),(3063,'2010-03-04 13:53:22',1,'Saida do sistema',1),(3064,'2010-03-04 14:01:53',1,'Entrada no sistema',1),(3065,'2010-03-04 14:02:05',1,'Saida do sistema',1),(3066,'2010-03-04 14:02:54',1,'Entrada no sistema',1),(3067,'2010-03-04 14:02:59',1,'Saida do sistema',1),(3068,'2010-03-04 14:03:40',1,'Entrada no sistema',1),(3069,'2010-03-04 14:03:53',1,'Saida do sistema',1),(3070,'2010-03-04 14:04:59',1,'Entrada no sistema',1),(3071,'2010-03-04 14:05:06',1,'Saida do sistema',1),(3072,'2010-03-04 14:05:40',1,'Entrada no sistema',1),(3073,'2010-03-04 14:05:44',1,'Saida do sistema',1),(3074,'2010-03-04 14:10:07',1,'Entrada no sistema',1),(3075,'2010-03-04 14:10:14',1,'Saida do sistema',1),(3076,'2010-03-04 14:12:27',1,'Entrada no sistema',1),(3077,'2010-03-04 14:12:47',1,'Saida do sistema',1),(3078,'2010-03-04 14:26:53',1,'Entrada no sistema',1),(3079,'2010-03-04 14:26:57',1,'Saida do sistema',1),(3080,'2010-03-04 14:27:21',1,'Entrada no sistema',1),(3081,'2010-03-04 14:27:27',1,'Saida do sistema',1),(3082,'2010-03-04 14:31:08',1,'Entrada no sistema',1),(3083,'2010-03-04 14:31:39',1,'Entrada no sistema',1),(3084,'2010-03-04 14:32:51',1,'Saida do sistema',1),(3085,'2010-03-04 14:34:33',1,'Entrada no sistema',1),(3086,'2010-03-04 14:37:03',1,'Saida do sistema',1),(3087,'2010-03-04 14:38:25',1,'Entrada no sistema',1),(3088,'2010-03-04 14:39:03',1,'Saida do sistema',1),(3089,'2010-03-04 14:39:15',1,'Entrada no sistema',1),(3090,'2010-03-04 14:40:20',1,'Saida do sistema',1),(3091,'2010-03-04 14:45:55',1,'Entrada no sistema',1),(3092,'2010-03-04 14:46:10',1,'Saida do sistema',1),(3093,'2010-03-04 14:46:32',1,'Entrada no sistema',1),(3094,'2010-03-04 14:46:44',1,'Saida do sistema',1),(3095,'2010-03-04 16:07:39',1,'Entrada no sistema',1),(3096,'2010-03-04 16:12:51',1,'Entrada no sistema',1),(3097,'2010-03-04 16:14:22',1,'Saida do sistema',1),(3098,'2010-03-04 16:30:13',1,'Entrada no sistema',1),(3099,'2010-03-04 17:16:40',1,'Saida do sistema',1),(3100,'2010-03-04 17:21:24',1,'Entrada no sistema',1),(3101,'2010-03-04 17:21:37',1,'Saida do sistema',1),(3102,'2010-03-04 17:22:36',1,'Entrada no sistema',1),(3103,'2010-03-04 17:22:48',1,'Saida do sistema',1),(3104,'2010-03-04 17:24:28',1,'Entrada no sistema',1),(3105,'2010-03-04 17:24:37',1,'Saida do sistema',1),(3106,'2010-03-04 17:25:19',1,'Entrada no sistema',1),(3107,'2010-03-04 17:25:24',1,'Saida do sistema',1),(3108,'2010-03-04 17:26:35',1,'Entrada no sistema',1),(3109,'2010-03-04 17:26:40',1,'Saida do sistema',1),(3110,'2010-03-04 17:30:07',1,'Entrada no sistema',1),(3111,'2010-03-04 17:30:29',1,'Saida do sistema',1),(3112,'2010-03-04 17:42:35',1,'Entrada no sistema',1),(3113,'2010-03-04 18:12:01',1,'Saida do sistema',1),(3114,'2010-03-04 18:21:11',1,'Entrada no sistema',1),(3115,'2010-03-04 18:21:22',1,'Saida do sistema',1),(3116,'2010-03-04 18:38:29',1,'Entrada no sistema',1),(3117,'2010-03-04 18:38:51',1,'Saida do sistema',1),(3118,'2010-03-04 18:41:48',1,'Entrada no sistema',1),(3119,'2010-03-04 18:49:03',1,'Saida do sistema',1),(3120,'2010-03-04 18:49:24',1,'Entrada no sistema',1),(3121,'2010-03-04 18:50:10',1,'Saida do sistema',1),(3122,'2010-03-04 18:53:07',1,'Entrada no sistema',1),(3123,'2010-03-04 19:02:52',1,'Saida do sistema',1),(3124,'2010-03-04 19:50:21',1,'Entrada no sistema',1),(3125,'2010-03-04 19:51:55',1,'Saida do sistema',1),(3126,'2010-03-04 19:52:01',1,'Entrada no sistema',1),(3127,'2010-03-04 19:52:07',1,'Saida do sistema',1),(3128,'2010-03-05 10:31:58',1,'Entrada no sistema',1),(3129,'2010-03-05 10:33:43',1,'Saida do sistema',1),(3130,'2010-03-05 10:38:24',1,'Entrada no sistema',1),(3131,'2010-03-05 10:38:32',1,'Saida do sistema',1),(3132,'2010-03-05 10:39:31',1,'Entrada no sistema',1),(3133,'2010-03-05 10:39:39',1,'Saida do sistema',1),(3134,'2010-03-05 10:40:15',1,'Entrada no sistema',1),(3135,'2010-03-05 10:40:20',1,'Saida do sistema',1),(3136,'2010-03-05 10:40:35',1,'Entrada no sistema',1),(3137,'2010-03-05 10:41:34',1,'Saida do sistema',1),(3138,'2010-03-05 10:42:08',1,'Entrada no sistema',1),(3139,'2010-03-05 10:42:14',1,'Saida do sistema',1),(3140,'2010-03-05 10:44:10',1,'Entrada no sistema',1),(3141,'2010-03-05 11:03:30',1,'Saida do sistema',1),(3142,'2010-03-05 11:04:23',1,'Entrada no sistema',1),(3143,'2010-03-05 11:04:31',1,'Saida do sistema',1),(3144,'2010-03-05 11:10:19',1,'Entrada no sistema',1),(3145,'2010-03-05 11:14:56',1,'Saida do sistema',1),(3146,'2010-03-05 11:20:31',1,'Entrada no sistema',1),(3147,'2010-03-05 11:25:14',1,'Saida do sistema',1),(3148,'2010-03-05 11:27:08',1,'Entrada no sistema',1),(3149,'2010-03-05 11:28:01',1,'Saida do sistema',1),(3150,'2010-03-05 11:28:24',1,'Entrada no sistema',1),(3151,'2010-03-05 11:29:09',1,'Saida do sistema',1),(3152,'2010-03-05 12:28:26',1,'Entrada no sistema',1),(3153,'2010-03-05 12:29:35',1,'Saida do sistema',1),(3154,'2010-03-05 12:29:57',1,'Entrada no sistema',1),(3155,'2010-03-05 12:30:33',1,'Saida do sistema',1),(3156,'2010-03-05 13:41:15',1,'Entrada no sistema',1),(3157,'2010-03-05 13:41:22',1,'Saida do sistema',1),(3158,'2010-03-05 13:42:50',1,'Entrada no sistema',1),(3159,'2010-03-05 13:43:11',1,'Saida do sistema',1),(3160,'2010-03-05 13:47:53',1,'Entrada no sistema',1),(3161,'2010-03-05 13:48:07',1,'Saida do sistema',1),(3162,'2010-03-05 14:04:59',1,'Entrada no sistema',1),(3163,'2010-03-05 14:05:08',1,'Produzindo 8 peÃ§as de 450 mm',2),(3164,'2010-03-05 14:08:46',1,'Entrada no sistema',1),(3165,'2010-03-05 14:08:55',1,'Produzindo 8 peÃ§as de 450 mm',2),(3166,'2010-03-05 14:09:58',1,'Entrada no sistema',1),(3167,'2010-03-05 14:10:01',1,'Produzindo 8 peÃ§as de 450 mm',2),(3168,'2010-03-05 14:10:07',1,'Saida do sistema',1),(3169,'2010-03-05 14:12:59',1,'Entrada no sistema',1),(3170,'2010-03-05 14:13:03',1,'Produzindo 8 peÃ§as de 450 mm',2),(3171,'2010-03-05 14:13:32',1,'Saida do sistema',1),(3172,'2010-03-05 14:16:17',1,'Entrada no sistema',1),(3173,'2010-03-05 14:16:29',1,'Produzindo 35 peÃ§as de 870 mm',2),(3174,'2010-03-05 14:16:38',1,'Saida do sistema',1),(3175,'2010-03-05 14:21:52',1,'Entrada no sistema',1),(3176,'2010-03-05 14:22:15',1,'Produzindo 5 peÃ§as de 800 mm',2),(3177,'2010-03-05 14:22:26',1,'Produzindo 5 peÃ§as de 120 mm',2),(3178,'2010-03-05 14:22:35',1,'Saida do sistema',1),(3179,'2010-03-05 14:34:53',1,'Entrada no sistema',1),(3180,'2010-03-05 14:36:00',1,'Saida do sistema',1),(3181,'2010-03-05 16:42:12',1,'Entrada no sistema',1),(3182,'2010-03-05 16:54:39',1,'Saida do sistema',1),(3183,'2010-03-05 16:54:48',1,'Entrada no sistema',1),(3184,'2010-03-05 16:54:56',1,'Saida do sistema',1),(3185,'2010-03-05 16:55:17',1,'Entrada no sistema',1),(3186,'2010-03-05 17:08:40',1,'Entrada no sistema',1),(3187,'2010-03-05 17:14:36',1,'Entrada no sistema',1),(3188,'2010-03-05 17:14:48',1,'Saida do sistema',1),(3189,'2010-03-05 17:15:17',1,'Entrada no sistema',1),(3190,'2010-03-05 17:44:49',1,'Entrada no sistema',1),(3191,'2010-03-05 18:01:59',1,'Entrada no sistema',1),(3192,'2010-03-05 18:02:05',1,'Saida do sistema',1),(3193,'2010-03-05 18:19:42',1,'Entrada no sistema',1),(3194,'2010-03-05 18:20:04',1,'Saida do sistema',1),(3195,'2010-03-05 18:20:16',1,'Entrada no sistema',1),(3196,'2010-03-05 18:20:21',1,'Saida do sistema',1),(3197,'2010-03-05 18:20:38',1,'Entrada no sistema',1),(3198,'2010-03-05 18:20:50',1,'Saida do sistema',1),(3199,'2010-03-05 18:27:13',1,'Entrada no sistema',1),(3200,'2010-03-05 18:27:19',1,'Saida do sistema',1),(3201,'2010-03-05 18:27:26',1,'Entrada no sistema',1),(3202,'2010-03-05 18:28:06',1,'Entrada no sistema',1),(3203,'2010-03-05 18:28:28',1,'Entrada no sistema',1),(3204,'2010-03-05 18:29:35',1,'Entrada no sistema',1),(3205,'2010-03-05 18:29:43',1,'Saida do sistema',1),(3206,'2010-03-05 18:33:12',1,'Entrada no sistema',1),(3207,'2010-03-05 18:33:22',1,'Saida do sistema',1),(3208,'2010-03-05 18:34:13',1,'Entrada no sistema',1),(3209,'2010-03-05 18:34:21',1,'Saida do sistema',1),(3210,'2010-03-05 18:36:23',1,'Entrada no sistema',1),(3211,'2010-03-05 18:36:29',1,'Saida do sistema',1),(3212,'2010-03-05 18:36:33',1,'Entrada no sistema',1),(3213,'2010-03-05 18:36:38',1,'Saida do sistema',1),(3214,'2010-03-05 18:36:43',1,'Entrada no sistema',1),(3215,'2010-03-05 18:36:51',1,'Saida do sistema',1),(3216,'2010-03-05 18:39:20',1,'Entrada no sistema',1),(3217,'2010-03-05 18:42:33',1,'Saida do sistema',1),(3218,'2010-03-05 19:00:58',1,'Entrada no sistema',1),(3219,'2010-03-05 19:01:06',1,'Saida do sistema',1),(3220,'2010-03-05 19:01:20',1,'Entrada no sistema',1),(3221,'2010-03-05 19:01:43',1,'Saida do sistema',1),(3222,'2010-03-05 19:02:13',1,'Entrada no sistema',1),(3223,'2010-03-05 19:19:58',1,'Entrada no sistema',1),(3224,'2010-03-05 19:20:15',1,'Saida do sistema',1),(3225,'2010-03-05 19:22:43',1,'Entrada no sistema',1),(3226,'2010-03-05 19:23:01',1,'Saida do sistema',1),(3227,'2010-03-05 19:38:58',1,'Entrada no sistema',1),(3228,'2010-03-05 19:39:22',1,'Saida do sistema',1),(3229,'2010-03-05 19:39:30',1,'Entrada no sistema',1),(3230,'2010-03-05 19:39:41',1,'Saida do sistema',1),(3231,'2010-03-05 19:43:25',1,'Entrada no sistema',1),(3232,'2010-03-05 19:43:44',1,'Saida do sistema',1),(3233,'2010-03-05 19:52:04',1,'Entrada no sistema',1),(3234,'2010-03-05 19:52:40',1,'Saida do sistema',1),(3235,'2010-03-05 20:01:34',1,'Entrada no sistema',1),(3236,'2010-03-05 20:01:40',1,'Saida do sistema',1),(3237,'2010-03-05 20:01:46',1,'Entrada no sistema',1),(3238,'2010-03-05 20:01:52',1,'Saida do sistema',1),(3239,'2010-03-05 20:02:35',1,'Entrada no sistema',1),(3240,'2010-03-05 20:02:45',1,'Saida do sistema',1),(3241,'2010-03-08 11:30:40',1,'Entrada no sistema',1),(3242,'2010-03-08 11:31:10',1,'Saida do sistema',1),(3243,'2010-03-08 13:50:43',1,'Entrada no sistema',1),(3244,'2010-03-08 13:50:49',1,'Saida do sistema',1),(3245,'2010-03-08 13:51:09',1,'Entrada no sistema',1),(3246,'2010-03-08 13:51:19',1,'Saida do sistema',1),(3247,'2010-03-08 13:51:47',1,'Entrada no sistema',1),(3248,'2010-03-08 13:51:54',1,'Saida do sistema',1),(3249,'2010-03-08 13:52:24',1,'Entrada no sistema',1),(3250,'2010-03-08 13:52:31',1,'Saida do sistema',1),(3251,'2010-03-08 14:22:05',1,'Entrada no sistema',1),(3252,'2010-03-08 14:22:12',1,'Saida do sistema',1),(3253,'2010-03-08 14:24:08',1,'Entrada no sistema',1),(3254,'2010-03-08 14:24:15',1,'Saida do sistema',1),(3255,'2010-03-08 14:30:30',1,'Entrada no sistema',1),(3256,'2010-03-08 14:30:43',1,'Saida do sistema',1),(3257,'2010-03-08 14:34:15',1,'Entrada no sistema',1),(3258,'2010-03-08 14:34:23',1,'Saida do sistema',1),(3259,'2010-03-08 14:35:15',1,'Entrada no sistema',1),(3260,'2010-03-08 14:35:21',1,'Saida do sistema',1),(3261,'2010-03-12 13:09:26',1,'Entrada no sistema',1),(3262,'2010-03-12 13:09:52',1,'Produzindo 50 peÃ§as de 1200 mm',2),(3263,'2010-03-12 13:10:26',1,'Saida do sistema',1),(3264,'2010-03-12 13:13:58',1,'Entrada no sistema',1),(3265,'2010-03-12 13:14:43',1,'Produzindo 50 peÃ§as de 1200 mm',2),(3266,'2010-03-12 13:20:44',1,'Produzindo 50 peÃ§as de 1200 mm',2),(3267,'2010-03-12 13:21:09',1,'Saida do sistema',1),(3268,'2010-03-12 13:22:49',1,'Entrada no sistema',1),(3269,'2010-03-12 13:23:01',1,'Produzindo 50 peÃ§as de 1200 mm',2),(3270,'2010-03-12 13:23:37',1,'Saida do sistema',1),(3271,'2010-03-12 13:26:17',1,'Entrada no sistema',1),(3272,'2010-03-12 13:26:33',1,'Produzindo 30 peÃ§as de 550 mm',2),(3273,'2010-03-12 13:29:40',1,'Saida do sistema',1),(3274,'2010-03-12 13:47:38',1,'Entrada no sistema',1),(3275,'2010-03-12 13:48:17',1,'Produzindo 26 peÃ§as de 550 mm',2),(3276,'2010-03-12 13:52:08',1,'Saida do sistema',1),(3277,'2010-03-12 13:52:30',1,'Entrada no sistema',1),(3278,'2010-03-12 13:52:47',1,'Produzindo 15 peÃ§as de 2000 mm',2),(3279,'2010-03-12 13:55:20',1,'Saida do sistema',1),(3280,'2010-03-12 13:59:26',1,'Entrada no sistema',1),(3281,'2010-03-12 13:59:41',1,'Produzindo 25 peÃ§as de 3240 mm',2),(3282,'2010-03-12 14:07:27',1,'Saida do sistema',1),(3283,'2010-03-12 14:25:59',1,'Entrada no sistema',1),(3284,'2010-03-12 14:26:26',1,'Produzindo 40 peÃ§as de 3760 mm',2),(3285,'2010-03-12 14:33:03',1,'Saida do sistema',1),(3286,'2010-03-12 14:58:55',1,'Entrada no sistema',1),(3287,'2010-03-12 14:59:09',1,'Produzindo 230 peÃ§as de 1000 mm',2),(3288,'2010-03-12 16:00:38',1,'Entrada no sistema',1),(3289,'2010-03-12 16:00:57',1,'Produzindo 230 peÃ§as de 1000 mm',2),(3290,'2010-03-12 16:36:39',1,'Entrada no sistema',1),(3291,'2010-03-12 17:19:05',1,'Saida do sistema',1),(3292,'2010-03-12 18:15:55',1,'Entrada no sistema',1),(3293,'2010-03-12 18:16:16',1,'Produzindo 230 peÃ§as de 1000 mm',2),(3294,'2010-03-12 18:27:07',1,'Entrada no sistema',1),(3295,'2010-03-12 18:27:50',1,'Produzindo 230 peÃ§as de 1000 mm',2),(3296,'2010-03-12 18:28:14',1,'Produzindo 230 peÃ§as de 1000 mm',2),(3297,'2010-03-12 18:29:19',1,'Produzindo 227 peÃ§as de 1000 mm',2),(3298,'2010-03-12 18:34:08',1,'Produzindo 227 peÃ§as de 1000 mm',2),(3299,'2010-03-12 18:35:55',1,'Saida do sistema',1),(3300,'2010-03-15 19:21:32',1,'Entrada no sistema',1),(3301,'2010-03-15 19:22:46',1,'Produzindo 1 peÃ§as de 300 mm',2),(3302,'2010-03-15 19:28:57',1,'Entrada no sistema',1),(3303,'2010-03-15 19:29:57',1,'Alterada configuraÃ§Ã£o da mÃ¡quina',4),(3304,'2010-03-15 19:30:31',1,'Produzindo 3 peÃ§as de 300 mm',2),(3305,'2010-03-15 19:30:59',1,'Saida do sistema',1),(3306,'2010-03-16 10:55:42',1,'Entrada no sistema',1),(3307,'2010-03-16 10:56:20',1,'Saida do sistema',1),(3308,'2010-03-16 10:57:23',1,'Entrada no sistema',1),(3309,'2010-03-16 10:57:48',1,'Saida do sistema',1),(3310,'2010-03-16 11:44:23',1,'Entrada no sistema',1),(3311,'2010-03-16 11:46:41',1,'Saida do sistema',1),(3312,'2010-03-16 12:11:06',1,'Entrada no sistema',1),(3313,'2010-03-16 12:11:34',1,'Saida do sistema',1),(3314,'2010-03-16 12:12:03',1,'Entrada no sistema',1),(3315,'2010-03-16 12:12:26',1,'Saida do sistema',1),(3316,'2010-03-16 12:17:12',1,'Entrada no sistema',1),(3317,'2010-03-16 12:17:30',1,'Saida do sistema',1);
/*!40000 ALTER TABLE `log` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `modelos`
--

DROP TABLE IF EXISTS `modelos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `modelos` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `nome` varchar(20) NOT NULL,
  `pilotar` tinyint(1) default '0',
  `passo` float NOT NULL,
  `tam_max` float unsigned NOT NULL,
  `tam_min` float unsigned NOT NULL default '0',
  `estado` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `nome` (`nome`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Cadastro de modelos de perfis';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `modelos`
--

LOCK TABLES `modelos` WRITE;
/*!40000 ALTER TABLE `modelos` DISABLE KEYS */;
INSERT INTO `modelos` VALUES (5,'PP',1,100,2300,0,1),(6,'Coluna',1,250,5000,0,1),(7,'N',1,100,7500,300,1),(8,'Perfil',0,250,2500,500,1),(9,'N3',1,40,10000,0,1),(10,'Coluna PP',1,1,8000,0,0);
/*!40000 ALTER TABLE `modelos` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `permissoes`
--

DROP TABLE IF EXISTS `permissoes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `permissoes` (
  `ID_user` int(11) unsigned NOT NULL COMMENT 'ID do usuário',
  `ID_perm` int(11) unsigned NOT NULL COMMENT 'ID da permissão',
  `valor` int(10) unsigned NOT NULL COMMENT 'Valor da permissão',
  PRIMARY KEY  (`ID_user`,`ID_perm`),
  KEY `IDX_perm` (`ID_perm`),
  CONSTRAINT `FK_USER` FOREIGN KEY (`ID_user`) REFERENCES `usuarios` (`ID`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `permissoes_ibfk_1` FOREIGN KEY (`ID_perm`) REFERENCES `lista_permissoes` (`ID`) ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Permissões dos usuários';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `permissoes`
--

LOCK TABLES `permissoes` WRITE;
/*!40000 ALTER TABLE `permissoes` DISABLE KEYS */;
INSERT INTO `permissoes` VALUES (1,5,1),(1,6,1),(1,7,1),(1,8,1),(8,5,1),(8,6,1),(8,7,0),(8,8,1);
/*!40000 ALTER TABLE `permissoes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tarefas`
--

DROP TABLE IF EXISTS `tarefas`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tarefas` (
  `ID` int(10) unsigned NOT NULL auto_increment,
  `ID_Modelo` int(10) unsigned NOT NULL,
  `ID_Cliente` int(10) unsigned NOT NULL,
  `Qtd` int(10) unsigned NOT NULL,
  `Tamanho` float unsigned NOT NULL,
  `Data` datetime NOT NULL,
  `Coments` text NOT NULL,
  `Pedido` varchar(20) default NULL,
  `Estado` smallint(5) unsigned default '0',
  `ID_User` int(10) unsigned NOT NULL,
  `QtdProd` int(10) unsigned default '0',
  `origem` int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  KEY `FK_Modelos` (`ID_Modelo`),
  KEY `FK_Clientes` (`ID_Cliente`),
  KEY `FK_Usuarios` (`ID_User`),
  CONSTRAINT `FK_Clientes` FOREIGN KEY (`ID_Cliente`) REFERENCES `clientes` (`ID`) ON UPDATE CASCADE,
  CONSTRAINT `FK_Modelos` FOREIGN KEY (`ID_Modelo`) REFERENCES `modelos` (`ID`) ON UPDATE CASCADE,
  CONSTRAINT `FK_Usuarios` FOREIGN KEY (`ID_User`) REFERENCES `usuarios` (`ID`) ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tarefas`
--

LOCK TABLES `tarefas` WRITE;
/*!40000 ALTER TABLE `tarefas` DISABLE KEYS */;
INSERT INTO `tarefas` VALUES (1,6,7,12,1500,'2007-04-18 16:35:00','Teste','',3,1,0,0),(3,7,8,10,2200,'2007-02-08 17:32:10','blablabla','154/2007',2,1,10,0),(4,7,1,5,1524,'2008-12-31 08:37:00','Testando cadastro de tarefas','Nada',3,1,0,0),(6,6,1,290,250,'2007-02-13 15:40:45','?','Miranda',3,1,62,0),(7,5,1,25,2200,'2007-02-14 11:01:27','','',3,1,0,0),(8,6,9,15,500,'2007-02-21 15:24:57','','124/07',3,1,0,0),(9,7,10,12,1400,'2001-01-20 23:12:00','Adicionado no ARM','54/2007',2,1,12,0),(10,5,1,10000,2000,'2001-01-14 00:46:06','','',3,1,2599,0),(11,5,1,100,1000,'2001-11-01 23:33:43','','',3,1,0,0),(12,6,1,10,4000,'2001-12-06 01:08:30','','',2,1,10,0),(13,5,1,5000,800,'2001-12-06 01:13:47','','',3,1,17,0),(14,5,1,5,200,'2001-12-06 01:26:04','','',2,1,5,0),(15,5,1,1000,100,'2001-12-07 23:13:18','','',3,1,123,0),(16,5,1,20,200,'2001-12-07 23:38:28','','',2,1,20,0),(17,9,1,1000,80,'2001-12-07 23:43:54','','',3,1,240,0),(18,5,1,15,100,'2001-12-08 00:08:07','','',2,1,15,0),(19,5,1,5,100,'2001-12-08 18:56:33','','',2,1,6,0),(20,10,1,20,2240,'2002-04-08 15:33:16','','',2,1,20,0),(21,10,1,2,2240,'2002-04-10 19:30:38','','',2,1,2,0),(22,10,1,6,4000,'2002-04-10 19:31:44','','',2,1,6,0),(23,10,1,132,6802,'2002-04-10 19:34:35','','',2,1,132,0),(24,10,1,10,2000,'2002-04-10 20:00:13','','',3,1,0,0),(25,10,1,100,2240,'2002-04-11 00:40:05','','',3,1,1,0),(26,10,1,8,1680,'2002-04-11 16:05:01','','pallet',2,1,8,0),(27,10,1,8,3282,'2002-04-11 16:06:10','','pallet',2,1,8,0),(28,10,11,1,6800,'2002-04-11 16:07:07','',']..]..',2,1,1,0),(29,10,1,50,1200,'2010-03-05 09:41:41','','',2,1,50,0),(30,10,1,5,800,'2010-03-05 09:42:31','','',2,1,5,0),(31,10,1,35,870,'2010-03-05 09:43:32','','',2,1,35,0),(32,10,1,8,450,'2010-03-05 11:18:18','','',2,1,8,0),(33,10,1,5,120,'2010-03-05 11:35:36','','',2,1,5,0),(34,10,1,50,1200,'2010-03-12 10:28:08','','',2,1,50,0),(35,10,1,50,1200,'2010-03-12 10:34:09','','',2,1,50,0),(36,10,1,50,1200,'2010-03-12 10:36:26','','',2,1,50,0),(37,10,1,30,550,'2010-03-12 10:39:58','','',2,1,30,0),(38,10,1,15,2000,'2010-03-12 11:06:11','','',2,1,15,0),(39,10,1,25,3240,'2010-03-12 11:13:06','','',2,1,25,0),(40,10,1,40,3760,'2010-03-12 11:39:51','','',2,1,40,0),(41,10,1,230,1000,'2010-03-12 12:12:34','','',2,1,230,0),(42,10,1,1,300,'2010-03-15 16:36:17','','',2,1,1,0),(43,10,1,3,300,'2010-03-15 16:44:03','','',2,1,3,0);
/*!40000 ALTER TABLE `tarefas` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `usuarios`
--

DROP TABLE IF EXISTS `usuarios`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `usuarios` (
  `ID` int(11) unsigned NOT NULL auto_increment COMMENT 'ID do usuário',
  `nome` varchar(20) character set latin1 collate latin1_general_cs default NULL,
  `senha` varchar(20) character set latin1 collate latin1_general_cs NOT NULL,
  `lembrete` varchar(20) character set latin1 collate latin1_general_cs default NULL,
  `login` varchar(10) character set latin1 collate latin1_general_cs NOT NULL,
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `login` USING BTREE (`login`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Cadastro de usuários';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `usuarios`
--

LOCK TABLES `usuarios` WRITE;
/*!40000 ALTER TABLE `usuarios` DISABLE KEYS */;
INSERT INTO `usuarios` VALUES (1,'Usuario Master','tuLTIwaQuWk','padrao','master'),(7,'Marcelo Biaggi','fSori6QMsw6','','Marcelo'),(8,'Marcelo Fonseca','cxjYJtbOsik','a de sempre...','utikawa');
/*!40000 ALTER TABLE `usuarios` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2010-03-16 10:10:44
