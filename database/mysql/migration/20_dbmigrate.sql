INSERT INTO DocTypes (name) VALUES ( 'Progress Payment Invoice' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes VALUES ('DocType', @dtId, 'PartialInvoice', 'true');

INSERT INTO DocTypes (name) VALUES ( 'Partial Invoice' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes VALUES ('DocType', @dtId, 'PartialInvoice', 'true');

INSERT INTO DocTypes (name) VALUES ( 'Final Invoice' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes VALUES ('DocType', @dtId, 'SubstractPartialInvoice', 'true');

INSERT INTO DocTypes (name) VALUES ( 'Abschlagsrechnung' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes VALUES ('DocType', @dtId, 'PartialInvoice', 'true');

INSERT INTO DocTypes (name) VALUES ( 'Teilrechnung' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes VALUES ('DocType', @dtId, 'PartialInvoice', 'true');

INSERT INTO DocTypes (name) VALUES ( 'Schlußrechnung' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes VALUES ('DocType', @dtId, 'SubstractPartialInvoice', 'true');

