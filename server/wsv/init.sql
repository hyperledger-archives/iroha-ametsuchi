CREATE TABLE IF NOT EXISTS account (
    account_id uuid PRIMARY KEY,
    name character varying(255) NOT NULL,
    quorum "char" NOT NULL
);
CREATE TABLE IF NOT EXISTS signatory (
    account_id uuid NOT NULL REFERENCES account,
    public_key bit(256) NOT NULL,
    PRIMARY KEY (account_id, public_key)
);
CREATE TABLE IF NOT EXISTS domain (
    domain_id uuid PRIMARY KEY,
    name character varying(45) NOT NULL,
    root_account_id uuid NOT NULL REFERENCES account
);
CREATE TABLE IF NOT EXISTS asset (
    asset_id uuid PRIMARY KEY,
    name character varying(45) NOT NULL,
    domain_id uuid NOT NULL REFERENCES domain
);
CREATE TABLE IF NOT EXISTS account_has_asset (
    account_id uuid NOT NULL REFERENCES account,
    asset_id uuid NOT NULL,
    amount character varying(256) NOT NULL,
    "precision" "char" NOT NULL,
    permissions bit varying NOT NULL,
    PRIMARY KEY (account_id, asset_id)
);
CREATE TABLE IF NOT EXISTS domain_has_account (
    domain_id uuid NOT NULL REFERENCES domain,
    account_id uuid NOT NULL REFERENCES account,
    permissions bit varying NOT NULL,
    PRIMARY KEY (domain_id, account_id)
);
