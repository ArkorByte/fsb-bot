CREATE TABLE config (
    guild_id BIGINT PRIMARY KEY NOT NULL,
    member_role BIGINT DEFAULT 0,
    stateless_role BIGINT DEFAULT 0,
    welcome_channel BIGINT DEFAULT 0,
    journalism_channel BIGINT DEFAULT 0
);

CREATE TABLE nations (
    nation_id VARCHAR(30) PRIMARY KEY NOT NULL,
    display_name VARCHAR(50) NOT NULL,
    description VARCHAR(500) DEFAULT 'No description.' NOT NULL,
    join_condition TINYINT DEFAULT 1,
    invite_permission TINYINT DEFAULT 0,
    claim_time BIGINT DEFAULT (UNIX_TIMESTAMP()) NOT NULL,
    tux_balance BIGINT DEFAULT 0,
    government_type TINYINT DEFAULT 0,
    ideology TINYINT DEFAULT 8,
    leadership_changes BIGINT DEFAULT 0,
    last_leadership_change BIGINT DEFAULT 0,
    government_changes BIGINT DEFAULT 0,
    last_government_change BIGINT DEFAULT 0,
    media_freedom TINYINT DEFAULT 100,
    media_posts BIGINT DEFAULT 0,
    last_post BIGINT DEFAULT 0,
    censored_posts BIGINT DEFAULT 0,
    media_whitelist BOOLEAN DEFAULT 0,
    media_blacklist BOOLEAN DEFAULT 0,
    last_manual_censorship BIGINT DEFAULT 0,
    last_automatic_censorship BIGINT DEFAULT 0,
    nuclear_state TINYINT DEFAULT 0,
    acquired_nuclear_time BIGINT DEFAULT 0,
    veto_state BOOLEAN DEFAULT 0,
    veto_usage_count BIGINT DEFAULT 0,
    last_veto_usage BIGINT DEFAULT 0,
    resolutions_count BIGINT DEFAULT 0,
    last_resolution BIGINT DEFAULT 0,
    passed_resolutions BIGINT DEFAULT 0,
    last_passed_resolution BIGINT DEFAULT 0
);

CREATE TABLE nationality (
    user_id BIGINT PRIMARY KEY NOT NULL,
    nation_id VARCHAR(30) DEFAULT 0,
    rank TINYINT DEFAULT 0,
    last_rank_update BIGINT DEFAULT 0,
    joining_time BIGINT DEFAULT 0,
    FOREIGN KEY (nation_id) REFERENCES nations(nation_id) ON DELETE SET NULL
);

CREATE TABLE relations (
    defining_nation VARCHAR(30) NOT NULL,
    targeted_nation VARCHAR(30) NOT NULL,
    relation_score TINYINT DEFAULT 50,
    PRIMARY KEY (defining_nation, targeted_nation),
    FOREIGN KEY (defining_nation) REFERENCES nations(nation_id) ON DELETE CASCADE,
    FOREIGN KEY (targeted_nation) REFERENCES nations(nation_id) ON DELETE CASCADE
);

CREATE TABLE sanctions (
    resolution_id BIGINT PRIMARY KEY AUTO_INCREMENT,
    pending BOOLEAN DEFAULT TRUE,
    nation_id VARCHAR(30) NOT NULL,
    sanctioned_nation VARCHAR(30) NOT NULL,
    sanction_type TINYINT,
    sanction_title VARCHAR(100) NOT NULL,
    sanction_details VARCHAR(500) NOT NULL,
    sanction_amount BIGINT NOT NULL,
    sanction_start BIGINT NOT NULL,
    sanction_duration VARCHAR(3) NOT NULL,
    sanction_end BIGINT NOT NULL,
    vote_start BIGINT DEFAULT (UNIX_TIMESTAMP()) NOT NULL,
    vote_duration VARCHAR(3) NOT NULL,
    vote_end BIGINT NOT NULL,
    votes_for TEXT DEFAULT '',
    votes_against TEXT DEFAULT '',
    FOREIGN KEY (nation_id) REFERENCES nations(nation_id) ON DELETE CASCADE,
    FOREIGN KEY (sanctioned_nation) REFERENCES nations(nation_id) ON DELETE CASCADE
);

CREATE TABLE laws (
    resolution_id VARCHAR(36) PRIMARY KEY NOT NULL,
    pending BOOLEAN DEFAULT TRUE,
    nation_id VARCHAR(30) NOT NULL,
    law_title VARCHAR(100) NOT NULL,
    law_details VARCHAR(500) NOT NULL,
    law_adoption_time BIGINT DEFAULT 0,
    vote_start BIGINT DEFAULT (UNIX_TIMESTAMP()) NOT NULL,
    vote_duration VARCHAR(3) NOT NULL,
    vote_end BIGINT NOT NULL,
    votes_for TEXT DEFAULT '',
    votes_against TEXT DEFAULT '',
    FOREIGN KEY (nation_id) REFERENCES nations(nation_id) ON DELETE CASCADE
);

CREATE TABLE un_membership (
    nation_id VARCHAR(30) PRIMARY KEY NOT NULL,
    pending BOOLEAN DEFAULT TRUE,
    invited_by VARCHAR(30) NOT NULL,
    joining_time BIGINT DEFAULT 0,
    vote_start BIGINT DEFAULT (UNIX_TIMESTAMP()) NOT NULL,
    vote_duration VARCHAR(3) NOT NULL,
    vote_end BIGINT NOT NULL,
    votes_for TEXT DEFAULT '',
    votes_against TEXT DEFAULT '',
    FOREIGN KEY (nation_id) REFERENCES nations(nation_id) ON DELETE CASCADE,
    FOREIGN KEY (invited_by) REFERENCES nations(nation_id) ON DELETE RESTRICT
);

CREATE TABLE journalism (
    user_id BIGINT PRIMARY KEY NOT NULL,
    status TINYINT
);
