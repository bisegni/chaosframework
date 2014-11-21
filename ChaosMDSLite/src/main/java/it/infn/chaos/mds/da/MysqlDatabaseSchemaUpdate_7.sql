CREATE TABLE `chaosms`.`device_class` (
  `id_device_class` INT(11) NOT NULL AUTO_INCREMENT,		
  `device_class` VARCHAR(64) NOT NULL,		 -- C/C++ device class 
  `device_class_alias` VARCHAR(64) NOT NULL DEFAULT "---",	 -- friend name 
  `device_class_interface` VARCHAR(64) DEFAULT "---", 	 -- abstract class interface   
  `device_source` INTEGER NOT NULL DEFAULT 0, 	 -- source can be by database 1 or by CU
  PRIMARY KEY (`id_device_class`,`device_class`),
  UNIQUE INDEX `iddevice_class_UNIQUE` (`id_device_class` ASC));

