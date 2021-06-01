-- MySQL dump 10.13  Distrib 5.6.19
--
-- Host: localhost    Database: control_inv
-- ------------------------------------------------------
-- Server version	5.6.17

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
-- Table structure for table `cHortaliza`
--

DROP TABLE IF EXISTS `cHortaliza`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cHortaliza` (
  `idHortaliza` int(11) NOT NULL AUTO_INCREMENT,
  `nombre` varchar(45) DEFAULT NULL,
  `phMax` float DEFAULT NULL,
  `phMin` float DEFAULT NULL,
  `ecMax` float DEFAULT NULL,
  `ecMin` float DEFAULT NULL,
  `tempMax` float DEFAULT NULL,
  `tempMin` float DEFAULT NULL,
  `personalizado` tinyint(1) DEFAULT '0',
  PRIMARY KEY (`idHortaliza`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `cHortaliza`
--

LOCK TABLES `cHortaliza` WRITE;
/*!40000 ALTER TABLE `cHortaliza` DISABLE KEYS */;
/*!40000 ALTER TABLE `cHortaliza` ENABLE KEYS */;
UNLOCK TABLES;


--
-- Table structure for table `pCultivoLector`
--

DROP TABLE IF EXISTS `pCultivoLector`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `pCultivoLector` (
  `idpCultivoLector` int(11) NOT NULL AUTO_INCREMENT,
  `idCultivo` int(11) DEFAULT NULL,
  `idLector` int(11) DEFAULT NULL,
  PRIMARY KEY (`idpCultivoLector`),
  KEY `fk_culLector_Cultivo_idx` (`idCultivo`),
  KEY `fk_culLector_Lector_idx` (`idLector`),
  CONSTRAINT `fk_cl_lector` FOREIGN KEY (`idLector`) REFERENCES `tLector` (`idtLector`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_cl_cultivo` FOREIGN KEY (`idCultivo`) REFERENCES `tCultivo` (`idtCultivo`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `pCultivoLector`
--

LOCK TABLES `pCultivoLector` WRITE;
/*!40000 ALTER TABLE `pCultivoLector` DISABLE KEYS */;
/*!40000 ALTER TABLE `pCultivoLector` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sUser`
--

DROP TABLE IF EXISTS `sUser`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sUser` (
  `idUser` int(11) NOT NULL AUTO_INCREMENT,
  `userName` varchar(45) DEFAULT NULL,
  `password` varchar(45) DEFAULT NULL,
  `celular` varchar(45) DEFAULT NULL,
  `mail` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`idUser`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sUser`
--

LOCK TABLES `sUser` WRITE;
/*!40000 ALTER TABLE `sUser` DISABLE KEYS */;
/*!40000 ALTER TABLE `sUser` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tAlarmas`
--

DROP TABLE IF EXISTS `tAlarmas`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tAlarmas` (
  `idtAlarmas` int(11) NOT NULL AUTO_INCREMENT,
  `idCultivo` int(11) DEFAULT NULL,
  `idActuador` int(11) DEFAULT NULL,
  `idLector` int(11) DEFAULT NULL,
  `fechaInicio` datetime DEFAULT NULL,
  `fechaFin` datetime DEFAULT NULL,
  `valor` float DEFAULT NULL,
  `severidad` float DEFAULT NULL,
  PRIMARY KEY (`idtAlarmas`),
  KEY `fk_alarma_cultivo_idx` (`idCultivo`),
  KEY `fk_alarma_lector_idx` (`idLector`),
  CONSTRAINT `fk_alarma_cultivo` FOREIGN KEY (`idCultivo`) REFERENCES `tCultivo` (`idtCultivo`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_alarma_lector` FOREIGN KEY (`idLector`) REFERENCES `tLector` (`idtLector`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tAlarmas`
--

LOCK TABLES `tAlarmas` WRITE;
/*!40000 ALTER TABLE `tAlarmas` DISABLE KEYS */;
/*!40000 ALTER TABLE `tAlarmas` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tCultivo`
--

DROP TABLE IF EXISTS `tCultivo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tCultivo` (
  `idtCultivo` int(11) NOT NULL AUTO_INCREMENT,
  `idHortaliza` int(11) DEFAULT NULL,
  `idUser` int(11) DEFAULT NULL,
  PRIMARY KEY (`idtCultivo`),
  KEY `fk_cultivo_hortaliza_idx` (`idHortaliza`),
  KEY `fk_cultivo_user_idx` (`idUser`),
  CONSTRAINT `fk_cultivo_hortaliza` FOREIGN KEY (`idHortaliza`) REFERENCES `cHortaliza` (`idHortaliza`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `fk_cultivo_user` FOREIGN KEY (`idUser`) REFERENCES `sUser` (`idUser`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tCultivo`
--

LOCK TABLES `tCultivo` WRITE;
/*!40000 ALTER TABLE `tCultivo` DISABLE KEYS */;
/*!40000 ALTER TABLE `tCultivo` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tLector`
--

DROP TABLE IF EXISTS `tLector`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tLector` (
  `idtLector` int(11) NOT NULL AUTO_INCREMENT,
  `nombre` varchar(45) DEFAULT NULL,
  `tipo` varchar(45) DEFAULT NULL,
  `idConexion` varchar(45) DEFAULT NULL,
  PRIMARY KEY (`idtLector`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tLector`
--

LOCK TABLES `tLector` WRITE;
/*!40000 ALTER TABLE `tLector` DISABLE KEYS */;
/*!40000 ALTER TABLE `tLector` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `tLogLector`
--

DROP TABLE IF EXISTS `tLogLector`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `tLogLector` (
  `idtLogLector` int(11) NOT NULL AUTO_INCREMENT,
  `idLector` int(11) DEFAULT NULL,
  `fecha` datetime DEFAULT NULL,
  `ph` float DEFAULT NULL,
  `ec` float DEFAULT NULL,
  `temp` float DEFAULT NULL,
  `humedad` float DEFAULT NULL,
  `temAgua` float DEFAULT NULL,
  `luz` float DEFAULT NULL,
  `foto` mediumblob,
  PRIMARY KEY (`idtLogLector`),
  KEY `fk_ll_Lector_idx` (`idLector`),
  CONSTRAINT `fk_ll_Lector` FOREIGN KEY (`idLector`) REFERENCES `tLector` (`idtLector`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `tLogLector`
--

LOCK TABLES `tLogLector` WRITE;
/*!40000 ALTER TABLE `tLogLector` DISABLE KEYS */;
/*!40000 ALTER TABLE `tLogLector` ENABLE KEYS */;
UNLOCK TABLES;



/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-04-08 13:16:34
