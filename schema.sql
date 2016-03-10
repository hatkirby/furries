DROP TABLE IF EXISTS `verbs`;
CREATE TABLE `verbs` (
  `verb_id` INTEGER PRIMARY KEY,
  `infinitive` VARCHAR(32) NOT NULL,
  `past_tense` VARCHAR(32) NOT NULL,
  `past_participle` VARCHAR(32) NOT NULL,
  `ing_form` VARCHAR(32) NOT NULL,
  `s_form` VARCHAR(32) NOT NULL
);

DROP TABLE IF EXISTS `groups`;
CREATE TABLE `groups` (
  `group_id` INTEGER PRIMARY KEY,
  `parent_id` INTEGER,
  FOREIGN KEY (`parent_id`) REFERENCES `groups`(`group_id`)
);

DROP TABLE IF EXISTS `frames`;
CREATE TABLE `frames` (
  `frame_id` INTEGER PRIMARY KEY,
  `group_id` INTEGER NOT NULL,
  `data` BLOB NOT NULL,
  FOREIGN KEY (`group_id`) REFERENCES `groups`(`group_id`)
);

DROP TABLE IF EXISTS `verb_groups`;
CREATE TABLE `verb_groups` (
  `verb_id` INTEGER NOT NULL,
  `group_id` INTEGER NOT NULL,
  FOREIGN KEY (`verb_id`) REFERENCES `verbs`(`verb_id`),
  FOREIGN KEY (`group_id`) REFERENCES `groups`(`group_id`)
);

DROP TABLE IF EXISTS `adjectives`;
CREATE TABLE `adjectives` (
  `adjective_id` INTEGER PRIMARY KEY,
  `adjective` VARCHAR(32) NOT NULL
);