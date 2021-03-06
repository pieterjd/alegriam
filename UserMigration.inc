<?php

/**
 * @file
 * Migrate users from old alegria website
 */

class UserMigration extends Migration {
	public function __construct() {
		// Always call the parent constructor first for basic setup
		parent::__construct();

		// With migrate_ui enabled, migration pages will indicate people involved in
		// the particular migration, with their role and contact info. We default the
		// list in the shared class; it can be overridden for specific migrations.
		$this->team = array(
		  new MigrateTeamMember('Pieter-Jan Drouillon', 'drouillon@hotmail.com', t('Migration Lead')),
		);
		$this->description = t('Migrate users from the source database to drupal users');
		
		
		
		//blogpost of using migrate module from D6 to D7
		//http://dtek.net/blog/drupal-6-to-drupal-7-via-migrate-2
		
		//SQL Source
		//step 1. Define external Database
		//cf https://drupal.org/node/1014558
		//from settings.php of old system
		
		//$db_url = 'mysql://alegria:my08ens1al@localhost/alegria';
		//$db_prefix = 'drupal_';
		//IMPORTANT: REMOVE http:// from host string!!!!
		
		//remote database information
		Database::addConnectionInfo('for_migration', 'default', array(
			'driver' => 'mysql',
			'database' => 'alegria',
			'username' => 'alegria',
			'password' => 'my08ens1al',
			'host' => 'mysql.webnet4u.be',
			'prefix' => 'drupal_',
		));
		
		
		//testing
		//dpm(print_r(Database::getConnectionInfo('for_migration')));
		//dpm(print_r(Database::getConnection('default','for_migration')));
		//dpm(print_r(Database::setActiveConnection('for_migration')));
		//Database::setActiveConnection('default');
		//step 2. set the source of the migration to a Query of the database
		/*
		$query = Database::getConnection('default','for_migration')
				->select('users','u')
			   ->fields('u');
			   //->condition('uid',1,'>'); //uid 1 is root/admin/whateva
		
		
		
		$source_fields = array(
			'uid' => t('The User ID'),
		);
		$this->source = new MigrateSourceSQL($query,$source_fields);
		*/
		//define columns in CSV file
		$columns = array(
			10 => array('access','access'),
			9 => array('created','created'),
			17 => array('data','data'),
			16 => array('init','init'),
			14 => array('language','language'),
			11 => array('login','login'),
			3 => array('mail','mail'),
			4 => array('mode','mode'),
			1 => array('name','name'),
			2 => array('pass','pass'),
			15 => array('picture','picture'),
			8 => array('signature','signature'),
			19 => array('signature_format','signature_format'),
			5 => array('sort','sort'),
			12 => array('status','status'),
			7 => array('theme','theme'),
			6 => array('threshold','threshold'),
			13 => array('timezone','timezone'),
			18 => array('timezone_name','timezone_name'),
			0 => array('uid','uid'),
		);
		$file_path = DRUPAL_ROOT.'/'.drupal_get_path('module', 'alegriam').'/users.csv';
		$this->source = new MigrateSourceCSV($file_path,
			$columns,
			array(
				'header_rows' => 1,
				'delimiter' => ';',
			)
		);
		
		
		
		//step 3. set the destination of the migration
		$this->destination = new MigrateDestinationUser(
			array(
				'md5_passwords' => TRUE,
			)
		);
		
		//step 4. Mapping of old system to new
		//mapping table will be on new drupal install
		$this->map=new MigrateSQLMap($this->machineName,
			array(
				'uid' => array(
					'type' => 'int',
					'unsigned' => TRUE,
					'not null' => TRUE,
					'description' => 'D6 Unique User ID',
					'alias' => 'u',
				)
			),
			MigrateDestinationUser::getKeySchema()
		);
		//step 5. map the old fields to the new one
		$this->addFieldMapping('is_new')->defaultValue(TRUE);
		//$this->addFieldMapping('uid', 'uid');
		$this->addFieldMapping('name', 'name');
		$this->addFieldMapping('pass', 'pass');
		$this->addFieldMapping('mail', 'mail');
		//$this->addFieldMapping('language')->defaultValue('');
		//$this->addFieldMapping('theme')->defaultValue('');
		$this->addFieldMapping('signature','signature');
		$this->addFieldMapping('signature_format')->defaultValue('filtered_html');
		$this->addFieldMapping('created', 'created');
		$this->addFieldMapping('access', 'access');
		$this->addFieldMapping('login', 'login');
		$this->addFieldMapping('status', 'status');
		//$this->addFieldMapping('picture')->defaultValue(0);
		$this->addFieldMapping('init', 'init');
		//$this->addFieldMapping('timezone')->defaultValue(NULL);
		$this->addFieldMapping('path')->issueGroup(t('DNM')); // DNM = Do Not Map
		$this->addFieldMapping('status', 'status');
		//skip these source fields
		$this->addUnmigratedSources(
			array(
				'mode',
				'sort',
				'threshold',
				'theme',
				//'signature',
				'signature_format',
				'timezone',
				'language',
				'picture',
				'data',
				'timezone_name',
			)
		);
		 
		//skip these destination fields
		$this->addUnmigratedDestinations(
			array(
				//'is_new',
				//'pass',
				'roles',
				'role_names',
				'theme',
				//'signature',
				//'signature_format',
				//'status',
				'timezone',
				'language',
				'picture',
				'pathauto',
				'data',
			)
		);
		
    
    
	}



}