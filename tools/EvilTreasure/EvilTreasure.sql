USE evildb;
GO
INSERT INTO CharacterItem (CID, ItemID, RegDate, RentDate, RentHourPeriod, cnt) (SELECT CID, '1000001', GETDATE(), GETDATE(), '99999', 1 FROM Character(nolock) WHERE FR = '0')