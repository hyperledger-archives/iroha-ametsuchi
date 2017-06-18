CREATE TABLE IF NOT EXISTS account (
    account_id char(32) PRIMARY KEY,
    quorum int NOT NULL,
    status int NOT NULL DEFAULT 0
);
CREATE TABLE IF NOT EXISTS signatory (
    account_id char(32) NOT NULL REFERENCES account,
    public_key char(32) NOT NULL,
    PRIMARY KEY (account_id, public_key)
);
CREATE TABLE IF NOT EXISTS peer (
    peer_id serial PRIMARY KEY,
    account_id char(32) NOT NULL REFERENCES account,
    address inet NOT NULL UNIQUE,
    state int NOT NULL DEFAULT 0
);
CREATE TABLE IF NOT EXISTS domain (
    domain_id character varying(164) PRIMARY KEY,
    parent_domain_id character varying(131) NOT NULL REFERENCES domain(domain_id),
    open bool NOT NULL
);
CREATE TABLE IF NOT EXISTS asset (
    asset_id character varying(197) PRIMARY KEY,
    domain_id character varying(164) NOT NULL REFERENCES domain,
    data json
);
CREATE TABLE IF NOT EXISTS exchange (
    asset1_id character varying(197) NOT NULL REFERENCES asset(asset_id),
    asset2_id character varying(197) NOT NULL REFERENCES asset(asset_id),
    asset1 int NOT NULL,
    asset2 int NOT NULL,
    PRIMARY KEY (asset1_id, asset2_id)
);
CREATE TABLE IF NOT EXISTS wallet (
    wallet_id uuid PRIMARY KEY,
    asset_id character varying(197),
    amount int NOT NULL,
    precision int NOT NULL,
    permissions bit varying NOT NULL
);
CREATE TABLE IF NOT EXISTS account_has_wallet (
    account_id char(32) NOT NULL REFERENCES account,
    wallet_id uuid NOT NULL REFERENCES wallet,
    permissions bit varying NOT NULL,
    PRIMARY KEY (account_id, wallet_id)
);
CREATE TABLE IF NOT EXISTS account_has_asset (
    account_id char(32) NOT NULL REFERENCES account,
    asset_id character varying(197) NOT NULL REFERENCES asset,
    permissions bit varying NOT NULL,
    PRIMARY KEY (account_id, asset_id)
);
CREATE TABLE IF NOT EXISTS domain_has_account (
    domain_id character varying(164) NOT NULL REFERENCES domain,
    account_id char(32) NOT NULL REFERENCES account,
    permissions bit varying NOT NULL,
    PRIMARY KEY (domain_id, account_id)
);
