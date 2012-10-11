-- phpMyAdmin SQL Dump
-- version 3.3.7deb5build0.10.10.1
-- http://www.phpmyadmin.net
--
-- Servidor: localhost
-- Tempo de Geração: Out 11, 2012 as 07:02 AM
-- Versão do Servidor: 5.1.61
-- Versão do PHP: 5.3.3-1ubuntu9.10

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Banco de Dados: `cv_integrado`
--

-- --------------------------------------------------------

--
-- Estrutura da tabela `clientes`
--

DROP TABLE IF EXISTS `clientes`;
CREATE TABLE IF NOT EXISTS `clientes` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `nome` varchar(20) NOT NULL,
  PRIMARY KEY (`ID`),
  UNIQUE KEY `nome` (`nome`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='Cadastro de Clientes' AUTO_INCREMENT=11 ;

--
-- Extraindo dados da tabela `clientes`
--

INSERT INTO `clientes` (`ID`, `nome`) VALUES
(8, 'Altamira'),
(7, 'C4'),
(10, 'Marcelo Miranda'),
(1, 'Nenhum'),
(9, 'Outro Cliente'),
(2, 'Tecnequip');

-- --------------------------------------------------------

--
-- Estrutura da tabela `log`
--

DROP TABLE IF EXISTS `log`;
CREATE TABLE IF NOT EXISTS `log` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Data` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `ID_Usuario` int(11) unsigned NOT NULL,
  `Evento` text NOT NULL,
  `Tipo` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`),
  KEY `FK_ID_Usuario` (`ID_Usuario`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='Registro de Eventos' AUTO_INCREMENT=16195 ;

--
-- Extraindo dados da tabela `log`
--

-- --------------------------------------------------------

--
-- Estrutura da tabela `modelos`
--

DROP TABLE IF EXISTS `modelos`;
CREATE TABLE IF NOT EXISTS `modelos` (
  `ID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `nome` varchar(20) NOT NULL,
  `pilotar` tinyint(1) DEFAULT '0',
  `passo` float NOT NULL,
  `tam_max` float unsigned NOT NULL,
  `tam_min` float unsigned NOT NULL DEFAULT '0',
  `estado` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`),
  UNIQUE KEY `nome` (`nome`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COMMENT='Cadastro de modelos de perfis' AUTO_INCREMENT=17 ;

--
-- Extraindo dados da tabela `modelos`
--

INSERT INTO `modelos` (`ID`, `nome`, `pilotar`, `passo`, `tam_max`, `tam_min`, `estado`) VALUES
(16, 'Sigma', 2, 1, 10000, 1, 0);

-- --------------------------------------------------------

--
-- Estrutura da tabela `OPERADOR`
--

DROP TABLE IF EXISTS `OPERADOR`;
CREATE TABLE IF NOT EXISTS `OPERADOR` (
  `ID` int(11) unsigned NOT NULL,
  `NOME` varchar(50) NOT NULL,
  `USUARIO` varchar(50) NOT NULL,
  `SENHA` varchar(50) NOT NULL,
  `LEMBRETE` varchar(50) DEFAULT NULL,
  `PERFIL` int(11) NOT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Extraindo dados da tabela `OPERADOR`
--

INSERT INTO `OPERADOR` (`ID`, `NOME`, `USUARIO`, `SENHA`, `LEMBRETE`, `PERFIL`) VALUES
(1, 'SISTEMA', '', '', '', 1),
(2, 'MARCELO UTIKAWA', 'utikawa', '9UgPRZJPtDs', 'default', 3);

-- --------------------------------------------------------

--
-- Estrutura da tabela `PERFIL`
--

DROP TABLE IF EXISTS `PERFIL`;
CREATE TABLE IF NOT EXISTS `PERFIL` (
  `ID` int(11) NOT NULL,
  `NOME` varchar(50) NOT NULL,
  `PERMISSAO` varchar(10) NOT NULL DEFAULT '----------',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Extraindo dados da tabela `PERFIL`
--

INSERT INTO `PERFIL` (`ID`, `NOME`, `PERMISSAO`) VALUES
(1, 'OPERADOR', '-WWRR-----'),
(2, 'SUPERVISOR', 'WWWRW-----'),
(3, 'ADMINISTRADOR', 'WWWWWWWWWW');

-- --------------------------------------------------------

--
-- Estrutura da tabela `SyncTable`
--

DROP TABLE IF EXISTS `SyncTable`;
CREATE TABLE IF NOT EXISTS `SyncTable` (
  `SyncID` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `SyncSQL` text CHARACTER SET latin1 COLLATE latin1_general_ci NOT NULL,
  PRIMARY KEY (`SyncID`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=200 ;

--
-- Extraindo dados da tabela `SyncTable`
--


--
-- Estrutura da tabela `tarefas`
--

DROP TABLE IF EXISTS `tarefas`;
CREATE TABLE IF NOT EXISTS `tarefas` (
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
  KEY `FK_Usuarios` (`ID_User`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=192 ;

--
-- Extraindo dados da tabela `tarefas`
--

--
-- Restrições para as tabelas dumpadas
--

--
-- Restrições para a tabela `log`
--
ALTER TABLE `log`
  ADD CONSTRAINT `log_ibfk_1` FOREIGN KEY (`ID_Usuario`) REFERENCES `OPERADOR` (`ID`);

--
-- Restrições para a tabela `tarefas`
--
ALTER TABLE `tarefas`
  ADD CONSTRAINT `tarefas_ibfk_1` FOREIGN KEY (`ID_User`) REFERENCES `OPERADOR` (`ID`),
  ADD CONSTRAINT `FK_Clientes` FOREIGN KEY (`ID_Cliente`) REFERENCES `clientes` (`ID`) ON UPDATE CASCADE,
  ADD CONSTRAINT `FK_Modelos` FOREIGN KEY (`ID_Modelo`) REFERENCES `modelos` (`ID`) ON UPDATE CASCADE;
