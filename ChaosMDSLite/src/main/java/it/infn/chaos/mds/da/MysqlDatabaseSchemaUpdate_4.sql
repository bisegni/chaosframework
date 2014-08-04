ALTER TABLE `chaosms`.`unit_server_cu_instance` 
DROP FOREIGN KEY `fk_unit_server_cu_instance_1`;
ALTER TABLE `chaosms`.`unit_server_cu_instance` 
ADD INDEX `fk_unit_server_cu_instance_1_idx` (`unit_server_alias` ASC, `cu_type` ASC);
ALTER TABLE `chaosms`.`unit_server_cu_instance` 
ADD CONSTRAINT `fk_unit_server_cu_instance_1`
  FOREIGN KEY (`unit_server_alias` , `cu_type`)
  REFERENCES `chaosms`.`unit_server_published_cu` (`unit_server_alias` , `control_unit_alias`)
  ON DELETE CASCADE
  ON UPDATE RESTRICT;