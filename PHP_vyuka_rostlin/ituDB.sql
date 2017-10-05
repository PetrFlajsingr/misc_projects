-- phpMyAdmin SQL Dump
-- version 3.4.5
-- http://www.phpmyadmin.net
--
-- Počítač: localhost
-- Vygenerováno: Ned 11. pro 2016, 00:49
-- Verze MySQL: 5.6.33
-- Verze PHP: 5.3.29

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Databáze: `xflajs00`
--
CREATE DATABASE `xflajs00` DEFAULT CHARACTER SET latin2 COLLATE latin2_czech_cs;
USE `xflajs00`;

-- --------------------------------------------------------

--
-- Struktura tabulky `PLANT`
--

CREATE TABLE IF NOT EXISTS `PLANT` (
  `PLANTID` int(11) NOT NULL,
  `NAME` varchar(255) COLLATE latin2_czech_cs NOT NULL,
  `TYPE` varchar(255) COLLATE latin2_czech_cs NOT NULL,
  `RARITY` varchar(20) COLLATE latin2_czech_cs NOT NULL,
  `HEIGHT` int(11) NOT NULL,
  `FRUIT` varchar(20) COLLATE latin2_czech_cs DEFAULT NULL,
  `FLOWER` varchar(20) COLLATE latin2_czech_cs DEFAULT NULL,
  `DESCR` varchar(255) COLLATE latin2_czech_cs DEFAULT NULL,
  `LINK` varchar(200) COLLATE latin2_czech_cs DEFAULT NULL,
  `vyskyt` varchar(30) COLLATE latin2_czech_cs NOT NULL DEFAULT 'Ceska Republika',
  `vyuziti` varchar(30) COLLATE latin2_czech_cs NOT NULL DEFAULT 'Zadne',
  PRIMARY KEY (`PLANTID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Vypisuji data pro tabulku `PLANT`
--

INSERT INTO `PLANT` (`PLANTID`, `NAME`, `TYPE`, `RARITY`, `HEIGHT`, `FRUIT`, `FLOWER`, `DESCR`, `LINK`, `vyskyt`, `vyuziti`) VALUES
(0, 'Tulipan', 'Jednodelozna', 'Obycejny', 60, 'Ne', 'Ruzne barvy', 'Tulipan (Tulipa) je rozsahly rod jednodeloznych rostlin z celedi liliovitych v uzsim pojeti (Liliaceae s. str.).', 'img/tulipan.jpg', 'Ceska Republika', 'Zadne'),
(1, 'Kopretina', 'Krytosemenna', 'Obycejna', 10, 'Ne', 'Bily', 'Kopretina (Leucanthemum Mill., 1754) je rod priblizne 33-70 druhu rostlin z celedi hvezdnicovitych (Asteraceae). Nazev kopretina patri mezi novotvary vytvorene v 19. stoleti Janem Svatoplukem Preslem. Vedecke jmeno je odvozeno od slov ?leukos? (bily) a ?a', 'img/kopretina.jpg', 'Ceska Republika', 'Zadne'),
(2, 'Violka', 'Krytosemenna', 'Obycejna', 7, 'Ne', 'Fialovy', 'Violka (Viola L.) je rod jednoletych az vytrvalych bylin. Listy jsou rapikate. Kvety jsou nekdy rozliseny na chasmogamicke (s barevnymi korunami a jsou opylovany hmyzem a kleistogamicke (zakrnele, zelenave, s autogamii) nebo nejsou rozliseny. ', 'img/violka.jpg', 'Ceska Republika', 'Zadne'),
(3, 'Slunecnice', 'Krytosemenna', 'Obycejne', 200, 'Ne', 'Zluty', 'Slunecnice (Helianthus L., 1753) je rod vysokych bylin z celedi hvezdnicovitych. Cely rod obsahuje priblizne 55 az 67 druhu, dva z nich, slunecnice rocni a slunecnice topinambur, se pestuji po celem svete.', 'img/slunecnice.jpg', 'Ceska Republika', 'Zadne'),
(4, 'Jahodnik', 'Bylina', 'Obycejny', 10, 'Jahody', 'Bily', 'Jahodnik (Fragaria) je rod rostlin z celedi ruzovite (Rosaceae). Zahrnuje asi 20 druhu rozsirenych predevsim v mirnem pasu severni polokoule.', 'img/jahodnik.jpg', 'Ceska Republika', 'Zadne'),
(5, 'Mata', 'Bylina', 'Obycejna', 50, 'Ne', 'Ruzovy', 'Mata (Mentha) je rod aromatickych bylin mokrych nebo vlhkych mist s typickymi chlupatymi ctyrhrannymi lodyhami s klasnatymi kvetenstvimi.', 'img/mata.jpg', 'Ceska Republika', 'Zadne'),
(6, 'Medunka lekarska', 'Bylina', 'Obycejna', 100, 'Ne', 'Ne', 'Medunka lekarska (Melissa officinalis) je leciva rostlina z celedi hluchavkovitych.', 'img/medunka lekarska.jpg', 'Ceska Republika', 'Zadne'),
(7, 'Hlohyne', 'Ker', 'Obycejna', 300, 'Cervene bobule', 'Bily', 'Hlohyne (Pyracantha) je rod rostlin patrici do celedi ruzovite (Rosaceae).', 'img/hlohyne.jpg', 'Ceska Republika', 'Zadne'),
(8, 'Ruze', 'Ker', 'Obycejna', 300, 'Sipky', 'Ruzne barvy', 'Ruze (Rosa) je rod kerovitych rostlin s vice nez 100 druhy, ktere se v prirode vyskytuji na severni polokouli, predevsim v oblasti mirneho pasu', 'img/ruze.jpg', 'Ceska Republika', 'Zadne'),
(9, 'Rulik zlomocny', 'Bylina', 'Obycejna', 160, 'Cerne bobule', 'Hnedofialovy', 'Rulik zlomocny (Atropa bella-donna) je statna, az 180 cm vysoka vytrvala bylina. Je povazovana za nejnebezpecnejsi stredoevropskou jedovatou rostlinu, odhaduje se, ze otravy rulikem zlomocnym tvori zhruba polovinu vsech vaznych otrav rostlinneho puvodu na', 'img/rulik zlomocny.jpg', 'Ceska Republika', 'Zadne');

-- --------------------------------------------------------

--
-- Struktura tabulky `PLECORD`
--

CREATE TABLE IF NOT EXISTS `PLECORD` (
  `PLANT` int(11) NOT NULL,
  `RECORD` int(11) NOT NULL,
  PRIMARY KEY (`PLANT`,`RECORD`),
  KEY `RECORD` (`RECORD`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Vypisuji data pro tabulku `PLECORD`
--

INSERT INTO `PLECORD` (`PLANT`, `RECORD`) VALUES
(0, 1),
(1, 1),
(2, 1),
(3, 1),
(4, 1),
(5, 1),
(6, 1),
(7, 1),
(8, 1),
(9, 1),
(0, 20),
(0, 22),
(1, 22),
(5, 22),
(0, 25),
(1, 25),
(6, 25);

-- --------------------------------------------------------

--
-- Struktura tabulky `RECORD`
--

CREATE TABLE IF NOT EXISTS `RECORD` (
  `RECORDID` int(11) NOT NULL AUTO_INCREMENT,
  `CORRANS` int(11) DEFAULT NULL,
  `WRNGANS` int(11) DEFAULT NULL,
  `USER` varchar(10) COLLATE latin2_czech_cs DEFAULT NULL,
  PRIMARY KEY (`RECORDID`),
  KEY `USER` (`USER`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs AUTO_INCREMENT=26 ;

--
-- Vypisuji data pro tabulku `RECORD`
--

INSERT INTO `RECORD` (`RECORDID`, `CORRANS`, `WRNGANS`, `USER`) VALUES
(1, 291, 226, 'ugandix'),
(2, 0, 0, 'xflajs00'),
(19, 0, 0, 'dsadasdasd'),
(20, 0, 0, 'test123'),
(21, 0, 0, 'geriatrix'),
(22, 18, 11, 'peta'),
(23, 0, 0, '12345'),
(24, 0, 0, 'rebeka'),
(25, 0, 0, 'dominik');

-- --------------------------------------------------------

--
-- Struktura tabulky `USER`
--

CREATE TABLE IF NOT EXISTS `USER` (
  `LOGIN` varchar(10) COLLATE latin2_czech_cs NOT NULL,
  `PASSWORD` varchar(20) COLLATE latin2_czech_cs NOT NULL,
  PRIMARY KEY (`LOGIN`)
) ENGINE=InnoDB DEFAULT CHARSET=latin2 COLLATE=latin2_czech_cs;

--
-- Vypisuji data pro tabulku `USER`
--

INSERT INTO `USER` (`LOGIN`, `PASSWORD`) VALUES
('dominik', 'dominik'),
('dsadasdasd', 'asdasdd'),
('geriatrix', '12345'),
('peta', '555555'),
('rebeka', 'rebeka'),
('test123', '12345'),
('ugandix', '12345'),
('xflajs00', '5evevuca'),
('12345', '12345');

--
-- Omezení pro exportované tabulky
--

--
-- Omezení pro tabulku `PLECORD`
--
ALTER TABLE `PLECORD`
  ADD CONSTRAINT `PLECORD_ibfk_1` FOREIGN KEY (`PLANT`) REFERENCES `PLANT` (`PLANTID`),
  ADD CONSTRAINT `PLECORD_ibfk_2` FOREIGN KEY (`RECORD`) REFERENCES `RECORD` (`RECORDID`);

--
-- Omezení pro tabulku `RECORD`
--
ALTER TABLE `RECORD`
  ADD CONSTRAINT `RECORD_ibfk_1` FOREIGN KEY (`USER`) REFERENCES `USER` (`LOGIN`);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
