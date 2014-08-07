ALTER TABLE `chaosms`.`unit_server` 
ADD COLUMN `private_key` VARCHAR(1024) NULL AFTER `unit_server_hb_time`,
ADD COLUMN `public_key` VARCHAR(1024) NULL AFTER `private_key`,
ADD COLUMN `reg_state` INT NULL AFTER `public_key`;
