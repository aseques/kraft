INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Offer'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Progress Payment Invoice'), 1 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Offer'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Partial Invoice'), 2 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Offer'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Final Invoice'), 3 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Acceptance of Order'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Progress Payment Invoice'), 1 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Acceptance of Order'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Partial Invoice'), 2 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Acceptance of Order'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Final Invoice'), 3 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Progress Payment Invoice'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Final Invoice'), 1 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Partial Invoice'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Final Invoice'), 1 );


INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Angebot'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Teilrechnung'), 1 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Angebot'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Abschlagsrechnung'), 2 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Angebot'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Schlussrechnung'), 3 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Auftragsbestätigung'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Teilrechnung'), 1 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Auftragsbestätigung'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Abschlagsrechnung'), 2 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Auftragsbestätigung'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Schlussrechnung'), 3 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Abschlagsrechnung'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Schlussrechnung'), 1 );

INSERT INTO DocTypeRelations VALUES( (SELECT docTypeID FROM DocTypes WHERE name='Teilrechnung'),
                                     (SELECT docTypeID FROM DocTypes WHERE name='Schlussrechnung'), 1 );
