
CREATE TABLE `chaosms`.`device_config` (
  `unique_id` VARCHAR(64) NOT NULL,
  `attribute_name` VARCHAR(64) NOT NULL,
  `default_value` VARCHAR(256) NULL,
  `max_value` VARCHAR(256) NULL,
  `min_value` VARCHAR(256) NULL,
  PRIMARY KEY (`unique_id`, `attribute_name`));
