# message Creating attributes table...
CREATE TABLE attributes (
  hostObject VARCHAR(255),
  hostId     INT NOT NULL,
  name       VARCHAR(255),
  value      MEDIUMTEXT,

  PRIMARY KEY( hostObject, hostId, name )
);

# message Creating attributes for archived documents
CREATE TABLE archPosAttribs (
  archPosAttribId INT NOT NULL AUTO_INCREMENT,
  archDocID  INT NOT NULL,
  name       VARCHAR(255),
  value      VARCHAR(255),

  PRIMARY KEY( archPosAttribId )
);

# message Adding position type and overall price ot archdocpositions
ALTER TABLE archdocpos ADD COLUMN kind VARCHAR(64) AFTER ordNumber;
ALTER TABLE archdocpos ADD COLUMN overallPrice DECIMAL(10,2) AFTER price;

# message Changing old kinds to Normal
UPDATE archdocpos SET kind = "Normal";

# message Calculating archive position price
UPDATE archdocpos SET overallPrice = ROUND( price * amount, 2);

# message Creating Document Type table
CREATE TABLE DocTypes (
  docTypeID INT NOT NULL AUTO_INCREMENT,
  name      VARCHAR(255),
  
  PRIMARY KEY( docTypeID )
);

# message Filling doc type attributes
INSERT INTO DocTypes (name) VALUES ( 'Offer' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes VALUES ('DocType', @dtId, 'AllowDemand', 'true');
INSERT INTO attributes VALUES ('DocType', @dtId, 'AllowAlternative', 'true');

INSERT INTO DocTypes (name) VALUES ( 'Acceptance of Order' );
SET @dtId := LAST_INSERT_ID();
INSERT INTO attributes VALUES ('DocType', @dtId, 'AllowDemand', 'true');
INSERT INTO attributes VALUES ('DocType', @dtId, 'AllowAlternative', 'true');

INSERT INTO DocTypes (name) VALUES ( 'Invoice' );

# message Drop an unused table archdocStates
DROP TABLE archdocStates;
