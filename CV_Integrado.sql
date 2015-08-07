-- MySQL dump 10.13  Distrib 5.6.25, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: cv_integrado
-- ------------------------------------------------------
-- Server version	5.6.25-0ubuntu0.15.04.1

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
-- Table structure for table `OPERADOR`
--

DROP TABLE IF EXISTS `OPERADOR`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `OPERADOR` (
  `ID` int(11) unsigned NOT NULL,
  `NOME` varchar(50) NOT NULL,
  `USUARIO` varchar(50) NOT NULL,
  `SENHA` varchar(50) NOT NULL,
  `LEMBRETE` varchar(50) DEFAULT NULL,
  `PERFIL` int(11) NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `OPERADOR`
--

LOCK TABLES `OPERADOR` WRITE;
/*!40000 ALTER TABLE `OPERADOR` DISABLE KEYS */;
INSERT INTO `OPERADOR` VALUES (1,'SISTEMA','','','',1),(2,'MARCELO UTIKAWA','utikawa','tuLTIwaQuWk','',3),(3,'IZAEL','izael','yMnCew.7oa2','',2);
/*!40000 ALTER TABLE `OPERADOR` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `PARAMETRO`
--

DROP TABLE IF EXISTS `PARAMETRO`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `PARAMETRO` (
  `LINHA` varchar(5) NOT NULL,
  `MAQUINA` varchar(10) NOT NULL,
  `GRUPO` varchar(30) NOT NULL,
  `PARAMETRO` varchar(50) NOT NULL,
  `VALOR_INT` int(11) NOT NULL DEFAULT '0',
  `VALOR_FLOAT` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`LINHA`,`MAQUINA`,`GRUPO`,`PARAMETRO`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `PARAMETRO`
--

LOCK TABLES `PARAMETRO` WRITE;
/*!40000 ALTER TABLE `PARAMETRO` DISABLE KEYS */;
INSERT INTO `PARAMETRO` VALUES ('MZAPL','MZAPLAN','Corte','TamFaca',0,0),('MZAPL','MZAPLAN','Encoder','FatorCorr',0,0),('MZAPL','MZAPLAN','Encoder','Perimetro',0,0),('MZAPL','MZAPLAN','Encoder','Precisao',0,0),('MZAPL','MZAPLAN','Perfil','AutoAcel',0,0),('MZAPL','MZAPLAN','Perfil','AutoDesacel',0,0),('MZAPL','MZAPLAN','Perfil','AutoVel',0,0),('MZAPL','MZAPLAN','Perfil','DiamRolo',0,0),('MZAPL','MZAPLAN','Perfil','FatorMaq',0,0),('MZAPL','MZAPLAN','Perfil','ManAcel',0,0),('MZAPL','MZAPLAN','Perfil','ManDesacel',0,0),('MZAPL','MZAPLAN','Perfil','ManVel',0,0),('PPPES','PPPES','Corte','TamFaca',0,0),('PPPES','PPPES','Encoder','FatorCorr',0,0),('PPPES','PPPES','Encoder','Perimetro',0,0),('PPPES','PPPES','Encoder','Precisao',0,0),('PPPES','PPPES','Perfil','AutoAcel',0,0),('PPPES','PPPES','Perfil','AutoDesacel',0,0),('PPPES','PPPES','Perfil','AutoVel',0,0),('PPPES','PPPES','Perfil','DiamRolo',0,0),('PPPES','PPPES','Perfil','FatorMaq',0,0),('PPPES','PPPES','Perfil','ManAcel',0,0),('PPPES','PPPES','Perfil','ManDesacel',0,0),('PPPES','PPPES','Perfil','ManVel',0,0),('TESTE','TESTE','Corte','TamFaca',0,0),('TESTE','TESTE','Encoder','FatorCorr',0,0.9979),('TESTE','TESTE','Encoder','Perimetro',400,0),('TESTE','TESTE','Encoder','Precisao',2500,0),('TESTE','TESTE','Perfil','AutoAcel',0,0.3),('TESTE','TESTE','Perfil','AutoDesacel',0,0.3),('TESTE','TESTE','Perfil','AutoVel',120,0),('TESTE','TESTE','Perfil','DiamRolo',100,0),('TESTE','TESTE','Perfil','FatorMaq',0,16.88),('TESTE','TESTE','Perfil','ManAcel',0,0.1),('TESTE','TESTE','Perfil','ManDesacel',0,0.1),('TESTE','TESTE','Perfil','ManVel',15,0),('TESTE','TESTE','Prensa','Ciclos',2,0),('TESTE','TESTE','Prensa','CiclosFerram',0,0),('TESTE','TESTE','Prensa','CiclosLub',0,0),('TESTE','TESTE','Prensa','Passo',0,0),('TESTE','TESTE','Prensa','Sentido',0,0),('TRDIA','TRDIA','Corte','TamFaca',0,0),('TRDIA','TRDIA','Diagonal','DistPrensaCorte',4000,0),('TRDIA','TRDIA','Diagonal','QtdFurosInterm',0,0),('TRDIA','TRDIA','Encoder','FatorCorr',0,1),('TRDIA','TRDIA','Encoder','Perimetro',400,0),('TRDIA','TRDIA','Encoder','Precisao',2048,0),('TRDIA','TRDIA','Perfil','AutoAcel',0,1),('TRDIA','TRDIA','Perfil','AutoDesacel',0,1),('TRDIA','TRDIA','Perfil','AutoVel',100,0),('TRDIA','TRDIA','Perfil','DiamRolo',120,0),('TRDIA','TRDIA','Perfil','FatorMaq',0,50),('TRDIA','TRDIA','Perfil','ManAcel',0,3),('TRDIA','TRDIA','Perfil','ManDesacel',0,1),('TRDIA','TRDIA','Perfil','ManVel',30,0);
/*!40000 ALTER TABLE `PARAMETRO` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `PERFIL`
--

DROP TABLE IF EXISTS `PERFIL`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `PERFIL` (
  `ID` int(11) NOT NULL,
  `NOME` varchar(50) NOT NULL,
  `PERMISSAO` varchar(10) NOT NULL DEFAULT '----------',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `PERFIL`
--

LOCK TABLES `PERFIL` WRITE;
/*!40000 ALTER TABLE `PERFIL` DISABLE KEYS */;
INSERT INTO `PERFIL` VALUES (1,'OPERADOR','-WWRR-----'),(2,'SUPERVISOR','WWWRW-----'),(3,'ADMINISTRADOR','WWWWWWWWWW');
/*!40000 ALTER TABLE `PERFIL` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ProgPrensa`
--

DROP TABLE IF EXISTS `ProgPrensa`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ProgPrensa` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `LINHA` varchar(5) NOT NULL,
  `MAQUINA` varchar(10) NOT NULL,
  `Nome` varchar(50) NOT NULL,
  `ModoSingelo` int(11) NOT NULL,
  `NumCiclos` int(11) NOT NULL,
  `PedirNumCiclos` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `Nome` (`Nome`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ProgPrensa`
--

LOCK TABLES `ProgPrensa` WRITE;
/*!40000 ALTER TABLE `ProgPrensa` DISABLE KEYS */;
INSERT INTO `ProgPrensa` VALUES (1,'TESTE','TESTE','Primeiro',0,0,0),(3,'TESTE','TESTE','BlaBla',0,0,1),(4,'TESTE','TESTE','Porta-Palete Pesado',0,0,1),(5,'TESTE','TESTE','Testando...',1,5,1),(6,'TESTE','TESTE','novo!',1,123,1);
/*!40000 ALTER TABLE `ProgPrensa` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `ProgPrensaPassos`
--

DROP TABLE IF EXISTS `ProgPrensaPassos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `ProgPrensaPassos` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ProgPrensaID` int(10) unsigned NOT NULL,
  `ProgPrensaAvanco` int(11) NOT NULL,
  `ProgPrensaPortas` int(10) unsigned NOT NULL DEFAULT '0',
  `Repeticoes` int(11) NOT NULL DEFAULT '1',
  `LigarPrensa` int(10) unsigned NOT NULL DEFAULT '1',
  `PedirAvanco` int(11) NOT NULL DEFAULT '0',
  `TextoAvanco` varchar(50) NOT NULL,
  `PedirRepeticoes` int(11) NOT NULL DEFAULT '0',
  `TextoRepeticoes` varchar(50) NOT NULL,
  PRIMARY KEY (`ID`),
  KEY `FK_ProgPrensaID` (`ProgPrensaID`),
  CONSTRAINT `ProgPrensaPassos_ibfk_1` FOREIGN KEY (`ProgPrensaID`) REFERENCES `ProgPrensa` (`ID`) ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=82 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `ProgPrensaPassos`
--

LOCK TABLES `ProgPrensaPassos` WRITE;
/*!40000 ALTER TABLE `ProgPrensaPassos` DISABLE KEYS */;
INSERT INTO `ProgPrensaPassos` VALUES (61,5,1250,0,2,1,1,'Avanco Inicial:',1,'bla'),(62,5,1200,0,17,1,0,'',1,'Numero de Avancos:'),(63,5,400,0,1,1,0,'',0,''),(64,6,1500,0,1,1,0,'',0,''),(65,6,1200,0,1,1,0,'',0,''),(66,6,400,2,1,1,0,'',0,''),(71,4,2400,5,1,1,1,'Passo do Estampo',0,''),(72,4,1000,2,1,1,0,'',0,''),(77,3,200,1,1,1,0,'',0,''),(78,3,250,2,1,1,0,'',0,''),(79,1,1200,0,2,1,0,'',0,''),(80,1,500,2,1,1,0,'',0,''),(81,1,1300,0,1,1,0,'',0,'');
/*!40000 ALTER TABLE `ProgPrensaPassos` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `SyncTable`
--

DROP TABLE IF EXISTS `SyncTable`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `SyncTable` (
  `SyncID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `SyncSQL` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  PRIMARY KEY (`SyncID`)
) ENGINE=InnoDB AUTO_INCREMENT=64 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `SyncTable`
--

LOCK TABLES `SyncTable` WRITE;
/*!40000 ALTER TABLE `SyncTable` DISABLE KEYS */;
/*!40000 ALTER TABLE `SyncTable` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `clientes`
--

DROP TABLE IF EXISTS `clientes`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `clientes` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `nome` varchar(20) NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `nome` (`nome`)
) ENGINE=InnoDB AUTO_INCREMENT=11 DEFAULT CHARSET=utf8 COMMENT='Cadastro de Clientes';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `clientes`
--

LOCK TABLES `clientes` WRITE;
/*!40000 ALTER TABLE `clientes` DISABLE KEYS */;
INSERT INTO `clientes` VALUES (8,'Altamira'),(7,'C4'),(10,'Marcelo Miranda'),(1,'Nenhum'),(9,'Outro Cliente'),(2,'Tecnequip');
/*!40000 ALTER TABLE `clientes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `log`
--

DROP TABLE IF EXISTS `log`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `log` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Data` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `ID_Usuario` int(11) unsigned NOT NULL,
  `Evento` text NOT NULL,
  `Tipo` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`),
  KEY `FK_ID_Usuario` (`ID_Usuario`),
  CONSTRAINT `log_ibfk_1` FOREIGN KEY (`ID_Usuario`) REFERENCES `OPERADOR` (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=19106 DEFAULT CHARSET=utf8 COMMENT='Registro de Eventos';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `log`
--

LOCK TABLES `log` WRITE;
/*!40000 ALTER TABLE `log` DISABLE KEYS */;
/*!40000 ALTER TABLE `log` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `modelos`
--

DROP TABLE IF EXISTS `modelos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `modelos` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `nome` varchar(20) NOT NULL,
  `pilotar` tinyint(1) DEFAULT '0',
  `passo` float NOT NULL,
  `tam_max` float unsigned NOT NULL,
  `tam_min` float unsigned NOT NULL DEFAULT '0',
  `estado` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `nome` (`nome`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=utf8 COMMENT='Cadastro de modelos de perfis';
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `modelos`
--

LOCK TABLES `modelos` WRITE;
/*!40000 ALTER TABLE `modelos` DISABLE KEYS */;
INSERT INTO `modelos` VALUES (5,'PP',1,100,2300,0,1),(6,'Coluna',1,250,5000,0,1),(7,'N',1,100,7500,300,1),(8,'Perfil',0,250,2500,500,1),(9,'N3',1,40,10000,0,1),(10,'Coluna PP',1,1,8000,0,1),(11,'Coluna Mezanino',1,20,8000,0,1),(12,'Coluna N',1,40,8000,0,0),(13,'teste',0,0,0,0,1),(14,'P1',0,0,0,0,1),(15,'mais um',0,0,0,0,1),(16,'Sigma',2,1,10000,1,0);
/*!40000 ALTER TABLE `modelos` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tarefas`
--

DROP TABLE IF EXISTS `tarefas`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tarefas` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `ID_Modelo` int(10) unsigned NOT NULL,
  `ID_Cliente` int(10) unsigned NOT NULL,
  `Qtd` int(10) unsigned NOT NULL,
  `Tamanho` float unsigned NOT NULL,
  `Data` datetime NOT NULL,
  `Coments` text NOT NULL,
  `Pedido` varchar(20) DEFAULT NULL,
  `OrdemProducao` int(11) DEFAULT '0',
  `RomNumero` int(11) DEFAULT '0',
  `RomItem` int(11) DEFAULT '0',
  `Estado` smallint(5) unsigned DEFAULT '0',
  `ID_User` int(11) unsigned NOT NULL,
  `QtdProd` int(10) unsigned DEFAULT '0',
  `origem` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`),
  KEY `FK_Modelos` (`ID_Modelo`),
  KEY `FK_Clientes` (`ID_Cliente`),
  KEY `FK_Usuarios` (`ID_User`),
  CONSTRAINT `FK_Clientes` FOREIGN KEY (`ID_Cliente`) REFERENCES `clientes` (`ID`) ON UPDATE CASCADE,
  CONSTRAINT `FK_Modelos` FOREIGN KEY (`ID_Modelo`) REFERENCES `modelos` (`ID`) ON UPDATE CASCADE,
  CONSTRAINT `tarefas_ibfk_1` FOREIGN KEY (`ID_User`) REFERENCES `OPERADOR` (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=207 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tarefas`
--

LOCK TABLES `tarefas` WRITE;
/*!40000 ALTER TABLE `tarefas` DISABLE KEYS */;
/*!40000 ALTER TABLE `tarefas` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-08-07  8:00:51
