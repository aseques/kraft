--- New document types for partial invoices and the final invoice.

--- Insert the new doc type only once
INSERT INTO DocTypes (name)
SELECT 'Progress Payment Invoice'
WHERE NOT EXISTS (SELECT 1 FROM DocTypes WHERE name = 'Progress Payment Invoice');

INSERT INTO attributes (hostObject, hostId, name, valueIsList) VALUES ('DocType',
        (SELECT docTypeID FROM DocTypes WHERE name='Progress Payment Invoice'), 'PartialInvoice', 0);
INSERT INTO attributeValues(attributeId, value) VALUES (
        (SELECT id FROM attributes WHERE hostObject='DocType' AND hostId=(SELECT docTypeID FROM DocTypes WHERE name='Progress Payment Invoice') AND name='PartialInvoice'), 'true' );

INSERT INTO DocTypes (name)
SELECT 'Partial Invoice'
WHERE NOT EXISTS (SELECT 1 FROM DocTypes WHERE name = 'Partial Invoice');

INSERT INTO attributes (hostObject, hostId, name, valueIsList) VALUES ('DocType',
        (SELECT docTypeID FROM DocTypes WHERE name='Partial Invoice'), 'PartialInvoice', 0);
INSERT INTO attributeValues(attributeId, value) VALUES (
        (SELECT id FROM attributes WHERE hostObject='DocType' AND hostId=(SELECT docTypeID FROM DocTypes WHERE name='Partial Invoice') AND name='PartialInvoice'), 'true' );

INSERT INTO DocTypes (name)
SELECT 'Final Invoice'
WHERE NOT EXISTS (SELECT 1 FROM DocTypes WHERE name = 'Final Invoice');

INSERT INTO attributes (hostObject, hostId, name, valueIsList) VALUES ('DocType',
        (SELECT docTypeID FROM DocTypes WHERE name='Final Invoice'), 'SubstractPartialInvoice', 0);
INSERT INTO attributeValues(attributeId, value) VALUES (
        (SELECT id FROM attributes WHERE hostObject='DocType' AND hostId=(SELECT docTypeID FROM DocTypes WHERE name='Final Invoice') AND name='SubstractPartialInvoice'), 'true' );

--- German translations
INSERT INTO DocTypes (name)
SELECT 'Abschlagsrechnung'
WHERE NOT EXISTS (SELECT 1 FROM DocTypes WHERE name = 'Abschlagsrechnung');

INSERT INTO attributes (hostObject, hostId, name, valueIsList) VALUES ('DocType',
        (SELECT docTypeID FROM DocTypes WHERE name='Abschlagsrechnung'), 'PartialInvoice', 0);
INSERT INTO attributeValues(attributeId, value) VALUES (
        (SELECT id FROM attributes WHERE hostObject='DocType' AND hostId=(SELECT docTypeID FROM DocTypes WHERE name='Abschlagsrechnung') AND name='PartialInvoice'), 'true' );

INSERT INTO DocTypes (name)
SELECT 'Teilrechnung'
WHERE NOT EXISTS (SELECT 1 FROM DocTypes WHERE name = 'Teilrechnung');

INSERT INTO attributes (hostObject, hostId, name, valueIsList) VALUES ('DocType',
        (SELECT docTypeID FROM DocTypes WHERE name='Teilrechnung'), 'PartialInvoice', 0);
INSERT INTO attributeValues(attributeId, value) VALUES (
        (SELECT id FROM attributes WHERE hostObject='DocType' AND hostId=(SELECT docTypeID FROM DocTypes WHERE name='Teilrechnung') AND name='PartialInvoice'), 'true' );

INSERT INTO DocTypes (name)
SELECT 'Schlussrechnung'
WHERE NOT EXISTS (SELECT 1 FROM DocTypes WHERE name = 'Schlussrechnung');

INSERT INTO attributes (hostObject, hostId, name, valueIsList) VALUES ('DocType',
        (SELECT docTypeID FROM DocTypes WHERE name='Schlussrechnung'), 'SubstractPartialInvoice', 0);
INSERT INTO attributeValues(attributeId, value) VALUES (
        (SELECT id FROM attributes WHERE hostObject='DocType' AND hostId=(SELECT docTypeID FROM DocTypes WHERE name='Schlussrechnung') AND name='SubstractPartialInvoice'), 'true' );
