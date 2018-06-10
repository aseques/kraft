--- Englische Teilrechnung
INSERT INTO DocTypes (name) VALUES ( 'Progress Payment Invoice' );
INSERT INTO attributes (hostObject, hostId, name, value) VALUES ('DocType', (SELECT docTypeID FROM DocTypes WHERE name="Progress Payment Invoice"), 'PartialInvoice', 'true');

INSERT INTO DocTypes (name) VALUES ( 'Partial Invoice' );
INSERT INTO attributes (hostObject, hostId, name, value) VALUES ('DocType', (SELECT docTypeID FROM DocTypes WHERE name="Partial Invoice"), 'PartialInvoice', 'true');

INSERT INTO DocTypes (name) VALUES ( 'Final Invoice' );
INSERT INTO attributes (hostObject, hostId, name, value) VALUES ('DocType', (SELECT docTypeID FROM DocTypes WHERE name="Final Invoice"), 'SubstractPartialInvoice', 'true');

--- Deutsche Abschlags- und Teilrechnung
INSERT INTO DocTypes (name) VALUES ( 'Abschlagsrechnung' );
INSERT INTO attributes (hostObject, hostId, name, value) VALUES ('DocType', (SELECT docTypeID FROM DocTypes WHERE name="Abschlagsrechnung"), 'PartialInvoice', 'true');

INSERT INTO DocTypes (name) VALUES ( 'Teilrechnung' );
INSERT INTO attributes (hostObject, hostId, name, value) VALUES ('DocType', (SELECT docTypeID FROM DocTypes WHERE name="Teilrechnung"), 'PartialInvoice', 'true');

INSERT INTO DocTypes (name) VALUES ( 'Schlußrechnung' );
INSERT INTO attributes (hostObject, hostId, name, value) VALUES ('DocType', (SELECT docTypeID FROM DocTypes WHERE name="Schlußrechnung"), 'SubstractPartialInvoice', 'true');
