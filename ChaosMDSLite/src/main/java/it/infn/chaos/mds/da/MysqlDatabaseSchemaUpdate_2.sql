CREATE TABLE `chaosms`.`unit_server_cu_instance` (
  `unit_server_alias` VARCHAR(64) NOT NULL,
  `cu_id` VARCHAR(64) NOT NULL,
  `cu_type` VARCHAR(64) NOT NULL,
  `cu_param` VARCHAR(256) NULL,
  `driver_init` MEDIUMTEXT NULL,
  `state` VARCHAR(64) NOT NULL,
  `auto_load` VARCHAR(1) NULL DEFAULT 0,
  PRIMARY KEY (`unit_server_alias`, `cu_id`),
  CONSTRAINT `fk_unit_server_cu_instance_1`
    FOREIGN KEY (`unit_server_alias`)
    REFERENCES `chaosms`.`unit_server` (`unit_server_alias`)
    ON DELETE CASCADE
    ON UPDATE RESTRICT);

ALTER TABLE `chaosms`.`unit_server_cu_instance` 
	ADD UNIQUE INDEX `cu_id_UNIQUE` (`cu_id` ASC);
