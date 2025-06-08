DROP USER IF EXISTS 'czdbuser'@'localhost';
DROP DATABASE ClimateCzar;
CREATE DATABASE ClimateCzar;
CREATE USER 'czdbuser'@'localhost' IDENTIFIED BY 'Cl!m@t3cZ4r';
USE ClimateCzar;
GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, INDEX, ALTER, LOCK TABLES, EXECUTE, CREATE ROUTINE, ALTER ROUTINE, TRIGGER ON `ClimateCzar`.* TO 'czdbuser'@'localhost';
FLUSH PRIVILEGES;

CREATE TABLE `DeviceGroups` (
  `ID` int(11) NOT NULL,
  `SecLevel` varchar(20) NOT NULL DEFAULT 'anonymous',
  `Name` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

INSERT INTO `DeviceGroups` (`ID`, `SecLevel`, `Name`) VALUES
(1, 'anonymous', '- Default Group');

ALTER TABLE `DeviceGroups` ADD PRIMARY KEY (`ID`);

ALTER TABLE `DeviceGroups` MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

CREATE TABLE `InputDevices` (
  `ID` int(11) NOT NULL,
  `DeviceName` varchar(50) DEFAULT NULL,
  `DeviceType` tinyint(4) DEFAULT 0,
  `Dashboard` tinyint(4) NOT NULL DEFAULT 1,
  `Position` int(11) NOT NULL DEFAULT 1,
  `SwitchSec` varchar(20) NOT NULL DEFAULT 'subscriber',
  `GroupID` int(11) NOT NULL DEFAULT 1,
  `Reading` float DEFAULT 0,
  `ReadingSuffix` varchar(50) DEFAULT NULL,
  `GraphColor` varchar(11) DEFAULT NULL,
  `BGlow` tinyint(4) NOT NULL DEFAULT 35,
  `BGmid` tinyint(4) NOT NULL DEFAULT 80,
  `BGhigh` tinyint(4) NOT NULL DEFAULT 90,
  `RawText` text DEFAULT NULL,
  `LastUpdate` timestamp NULL DEFAULT NULL,
  `SensorList` varchar(255) DEFAULT NULL,
  `SwitchList` varchar(255) DEFAULT NULL,
  `ScheduleList` text DEFAULT NULL,
  `ReadCommand` varchar(255) DEFAULT NULL,
  `Notes` text DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

ALTER TABLE `InputDevices` ADD PRIMARY KEY (`ID`);

ALTER TABLE `InputDevices` MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT;

CREATE TABLE `InputHistory` (
  `ID` int(11) NOT NULL,
  `DeviceID` int(11) DEFAULT NULL,
  `TimeStamp` timestamp NULL DEFAULT NULL,
  `Reading` float DEFAULT NULL,
  `RawText` text DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

ALTER TABLE `InputHistory` ADD PRIMARY KEY (`ID`);

ALTER TABLE `InputHistory` MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT;

CREATE TABLE `OutputSwitches` (
  `ID` int(11) NOT NULL,
  `DeviceName` varchar(50) DEFAULT NULL,
  `InputType` tinyint(4) DEFAULT 0,
  `Dashboard` tinyint(4) NOT NULL DEFAULT 1,
  `Position` int(11) NOT NULL DEFAULT 1,
  `SwitchSec` varchar(20) NOT NULL DEFAULT 'subscriber',
  `GroupID` int(11) NOT NULL DEFAULT 1,
  `InputID` int(11) DEFAULT 0,
  `OpMode` tinyint(4) DEFAULT 0,
  `OverrideID` int(11) DEFAULT 0,
  `OverrideStatus` tinyint(4) DEFAULT 1,
  `Polarity` tinyint(4) DEFAULT 0,
  `HighValue` float DEFAULT 1,
  `LowValue` float DEFAULT 0,
  `Status` tinyint(4) DEFAULT 0,
  `Manual` tinyint(4) NOT NULL DEFAULT 0,
  `OneShot` tinyint(4) DEFAULT 1,
  `LastUpdate` timestamp NULL DEFAULT NULL,
  `GraphColor` varchar(11) DEFAULT NULL,
  `OnCommand` varchar(255) DEFAULT NULL,
  `OffCommand` varchar(255) DEFAULT NULL,
  `Notes` text DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

ALTER TABLE `OutputSwitches` ADD PRIMARY KEY (`ID`);

ALTER TABLE `OutputSwitches` MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT;

CREATE TABLE `OutputHistory` (
  `ID` int(11) NOT NULL,
  `DeviceID` int(11) DEFAULT NULL,
  `TimeStamp` timestamp NULL DEFAULT NULL,
  `Status` tinyint(4) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

ALTER TABLE `OutputHistory` ADD PRIMARY KEY (`ID`);

ALTER TABLE `OutputHistory` MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT;

CREATE TABLE `Users` (
  `ID` int(11) NOT NULL,
  `UN` varchar(50) DEFAULT NULL,
  `PW` varchar(50) DEFAULT NULL,
  `SecLevel` varchar(20) DEFAULT NULL,
  `Phone` varchar(15) DEFAULT NULL,
  `LockedOut` tinyint(4) NOT NULL DEFAULT 0,
  `LastLogin` timestamp NULL DEFAULT NULL,
  `LastActivity` timestamp NULL DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

INSERT INTO `Users` (`ID`, `UN`, `PW`, `SecLevel`, `Phone`, `LockedOut`, `LastLogin`, `LastActivity`) VALUES
(1, 'czadmin', 'cb13c33aa3e6cb5ea3062b1e4985588f', 'administrator', '8005551212', 0, '2025-06-07 22:00:04', '2025-06-08 02:31:22');

ALTER TABLE `Users` ADD PRIMARY KEY (`ID`);

ALTER TABLE `Users` MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

COMMIT;
