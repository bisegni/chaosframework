CREATE TABLE `chaosms`.`unit_server` (
  `unit_server_alias` VARCHAR(64) NOT NULL,
  `unit_server_ip_port` VARCHAR(256) NULL,
  `unit_server_hb_time` DATETIME NULL,
  PRIMARY KEY (`unit_server_alias`),
  UNIQUE INDEX `idunit_server_UNIQUE` (`unit_server_alias` ASC));

  CREATE TABLE `chaosms`.`unit_server_published_cu` (
  `unit_server_alias` VARCHAR(64) NOT NULL,
  `control_unit_alias` VARCHAR(64) NOT NULL,
  PRIMARY KEY (`unit_server_alias`, `control_unit_alias`),
  CONSTRAINT `fk_unit_server_publisched_cu_1`
    FOREIGN KEY (`unit_server_alias`)
    REFERENCES `chaosms`.`unit_server` (`unit_server_alias`)
    ON DELETE CASCADE
    ON UPDATE RESTRICT);
