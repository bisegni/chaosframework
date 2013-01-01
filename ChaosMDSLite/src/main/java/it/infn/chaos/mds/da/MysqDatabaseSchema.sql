CREATE DATABASE  IF NOT EXISTS `chaosms` /*!40100 DEFAULT CHARACTER SET utf8 */;
USE `chaosms`;
-- MySQL dump 10.13  Distrib 5.5.16, for osx10.5 (i386)
--
-- Host: liwaxbignap    Database: chaosms
-- ------------------------------------------------------
-- Server version	5.1.36-log

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
-- Table structure for table `dataset_attribute`
--

DROP TABLE IF EXISTS `dataset_attribute`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dataset_attribute` (
  `id_device` int(11) NOT NULL,
  `id_dataset` int(11) NOT NULL,
  `version` int(11) NOT NULL DEFAULT '1',
  `id_attribute` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(40) DEFAULT NULL,
  `description` varchar(256) DEFAULT NULL,
  `type` int(11) DEFAULT NULL,
  `range_max` varchar(40) DEFAULT NULL,
  `range_min` varchar(40) DEFAULT NULL,
  `direction` int(11) DEFAULT NULL,
  `attribute_check` varchar(128) DEFAULT NULL,
  `timestamp` datetime NOT NULL,
  `default_value` varchar(128) DEFAULT NULL,
  `tags_path` varchar(128) DEFAULT NULL,
  PRIMARY KEY (`id_device`,`id_dataset`,`id_attribute`,`version`),
  KEY `fk_dataset_description_dataset1` (`id_device`,`id_dataset`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `data_server`
--

DROP TABLE IF EXISTS `data_server`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `data_server` (
  `id_server` int(11) NOT NULL AUTO_INCREMENT,
  `hostname` varchar(45) NOT NULL DEFAULT 'localhost',
  `port` int(11) NOT NULL DEFAULT '8888',
  `is_live` varchar(1) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id_server`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `devices`
--

DROP TABLE IF EXISTS `devices`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `devices` (
  `id_device` int(11) NOT NULL AUTO_INCREMENT,
  `device_identification` varchar(40) NOT NULL,
  `cu_instance` varchar(40) NOT NULL,
  `net_address` varchar(40) NOT NULL,
  `init_at_startup` varchar(1) NOT NULL DEFAULT '0',
  `last_hb` datetime DEFAULT NULL,
  PRIMARY KEY (`id_device`),
  UNIQUE KEY `device_instance_UNIQUE` (`device_identification`),
  UNIQUE KEY `cu_instance_UNIQUE` (`cu_instance`)
) ENGINE=MyISAM AUTO_INCREMENT=13 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `dataset`
--

DROP TABLE IF EXISTS `dataset`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `dataset` (
  `id_device` int(11) NOT NULL,
  `id_dataset` int(11) NOT NULL AUTO_INCREMENT,
  `timestamp` datetime DEFAULT NULL,
  PRIMARY KEY (`id_dataset`,`id_device`)
) ENGINE=MyISAM AUTO_INCREMENT=20 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping events for database 'chaosms'
--

--
-- Dumping routines for database 'chaosms'
--
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2012-12-09 15:20:37
