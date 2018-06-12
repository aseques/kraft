INSERT INTO DocTypes (name) VALUES ( 'Progress Payment Invoice' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes (hostObject, hostId, name, valueIsList) VALUES ('DocType', @dtId, 'PartialInvoice', 0);
SET @aId := LAST_INSERT_ID();
INSERT INTO attributeValues(attributeId, value) VALUES ( @aId, 'true' );

INSERT INTO DocTypes (name) VALUES ( 'Partial Invoice' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes (hostObject, hostId, name, valueIsList) VALUES ('DocType', @dtId, 'PartialInvoice', 0);
SET @aId := LAST_INSERT_ID();
INSERT INTO attributeValues(attributeId, value) VALUES ( @aId, 'true' );

INSERT INTO DocTypes (name) VALUES ( 'Final Invoice' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes (hostObject, hostId, name, valueIsList) VALUES ('DocType', @dtId, 'SubstractPartialInvoice', 0);
SET @aId := LAST_INSERT_ID();
INSERT INTO attributeValues(attributeId, value) VALUES ( @aId, 'true' );

INSERT INTO DocTypes (name) VALUES ( 'Abschlagsrechnung' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes (hostObject, hostId, name, valueIsList) VALUES ('DocType', @dtId, 'PartialInvoice', 0);
SET @aId := LAST_INSERT_ID();
INSERT INTO attributeValues(attributeId, value) VALUES ( @aId, 'true' );

INSERT INTO DocTypes (name) VALUES ( 'Teilrechnung' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes (hostObject, hostId, name, valueIsList) VALUES ('DocType', @dtId, 'PartialInvoice', 0);
SET @aId := LAST_INSERT_ID();
INSERT INTO attributeValues(attributeId, value) VALUES ( @aId, 'true' );

INSERT INTO DocTypes (name) VALUES ( 'Schlussrechnung' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes (hostObject, hostId, name, valueIsList) VALUES ('DocType', @dtId, 'SubstractPartialInvoice', 0);
SET @aId := LAST_INSERT_ID();
INSERT INTO attributeValues(attributeId, value) VALUES ( @aId, 'true' );

