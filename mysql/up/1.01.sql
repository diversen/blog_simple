DROP TABLE IF EXISTS `blog_simple`;

CREATE TABLE `blog_simple` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `title` varchar(128) NOT NULL,
  `content` text,
  `updated` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `user_id` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=UTF8;