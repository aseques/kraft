# Follower definition for partial invoices and final invoice

SELECT @item := docTypeID FROM DocTypes WHERE name="Offer";
SELECT @follower := docTypeID FROM DocTypes WHERE name="Progress Payment Invoice";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 1 );

SELECT @follower := docTypeID FROM DocTypes WHERE name="Partial Invoice";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 2 );

SELECT @follower := docTypeID FROM DocTypes WHERE name="Final Invoice";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 3 );

SELECT @item := docTypeID FROM DocTypes WHERE name="Acceptance of Order";
SELECT @follower := docTypeID FROM DocTypes WHERE name="Progress Payment Invoice";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 1 );

SELECT @follower := docTypeID FROM DocTypes WHERE name="Partial Invoice";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 2 );

SELECT @follower := docTypeID FROM DocTypes WHERE name="Final Invoice";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 3 );

SELECT @item := docTypeID FROM DocTypes WHERE name="Progress Payment Invoice";
SELECT @follower := docTypeID FROM DocTypes WHERE name="Final Invoice";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 1 );

SELECT @item := docTypeID FROM DocTypes WHERE name="Partial Invoice";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 2 );

## Deutsche Übersetzung
SELECT @item := docTypeID FROM DocTypes WHERE name="Angebot";
SELECT @follower := docTypeID FROM DocTypes WHERE name="Abschlagsrechnung";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 1 );

SELECT @follower := docTypeID FROM DocTypes WHERE name="Teilrechnung";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 2 );

SELECT @follower := docTypeID FROM DocTypes WHERE name="Schlussrechnung";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 3 );

SELECT @item := docTypeID FROM DocTypes WHERE name="Auftragsbestätigung";
SELECT @follower := docTypeID FROM DocTypes WHERE name="Abschlagsrechnung";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 1 );

SELECT @follower := docTypeID FROM DocTypes WHERE name="Teilrechnung";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 2 );

SELECT @follower := docTypeID FROM DocTypes WHERE name="Schlussrechnung";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 3 );

SELECT @item := docTypeID FROM DocTypes WHERE name="Abschlagsrechnung";
SELECT @follower := docTypeID FROM DocTypes WHERE name="Schlussrechnung";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 1 );

SELECT @item := docTypeID FROM DocTypes WHERE name="Teilrechnung";
INSERT INTO DocTypeRelations VALUES( @item, @follower, 2 );
