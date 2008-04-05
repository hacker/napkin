CREATE TABLE sleeps (
	s_tobed text NOT NULL,			-- w3cish timestamp with minute precision
	s_alarm text NOT NULL PRIMARY KEY,	-- w3cish timestamp with minute precision
	s_window integer NOT NULL,		-- number of minutes
	s_data_a integer NOT NULL,		-- number of seconds
	s_almost_awakes text NOT NULL,		-- [^0-9:TZ-]-separated list of w3cish
						-- timestamps with second precision
	s_timezone integer NOT NULL		-- seconds west of GMT, TODO: make use of it
);
