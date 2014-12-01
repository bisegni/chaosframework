ALTER TABLE `chaosms`.`device_config` RENAME TO  `chaosms`.`attribute_config` ;
ALTER TABLE `chaosms`.`unit_server_cu_instance` ADD COLUMN `driver_spec` MEDIUMTEXT NULL  AFTER `driver_init` ;
