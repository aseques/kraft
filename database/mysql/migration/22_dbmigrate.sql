# Add a predecessor column to the documents table

ALTER TABLE document ADD COLUMN predecessor VARCHAR(32) AFTER projectLabel;
ALTER TABLE archdoc  ADD COLUMN predecessor VARCHAR(32) AFTER projectLabel;

