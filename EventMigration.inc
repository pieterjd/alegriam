<?php

/**
 * @file
 * Migrate events from old alegria website
 */

class EventMigration extends Migration {
	public function __construct() {
		// Always call the parent constructor first for basic setup
		parent::__construct();

		// With migrate_ui enabled, migration pages will indicate people involved in
		// the particular migration, with their role and contact info. We default the
		// list in the shared class; it can be overridden for specific migrations.
		$this->team = array(
		  new MigrateTeamMember('Pieter-Jan Drouillon', 'drouillon@hotmail.com', t('Migration Lead')),
		);
		$this->description = t('Migrate events from the source database to drupal users');
		
		
		
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
		
		//dpm('event migration called');
		
		//step 2. set the source of the migration to a CSV file
		
		//source columns
		$columns = array(
			0 => array('title','title of event'),
			1 => array('body','body'),
			2 => array('nid','node id'),
			3 => array('postdate','postdate'),
			4 => array('published','published?'),
			5 => array('startdate','Start date'),
			6 => array('enddate','enddate'),
			7 => array('term','event term'),
			8 => array('locationname','location name'),
			9 => array('uid','User id'),
			10 => array('username','username'),
		);
		$file_path = DRUPAL_ROOT.'/'.drupal_get_path('module', 'alegriam').'/evenement.csv';
		$this->source = new MigrateSourceCSV($file_path,
			$columns,
			array(
				'header_rows' => 1,
				'delimiter' => ';',
			)
		);
		//step 3. set the destination of the migration
		$this->destination = new MigrateDestinationNode('event');
			
		
		//step 4. Mapping of old system to new
		//mapping table will be on new drupal install
		$this->map=new MigrateSQLMap($this->machineName,
			array(
				'nid' => array(
					'type' => 'int',
					'unsigned' => TRUE,
					'not null' => TRUE,
					'description' => 'D6 Unique Node ID',
					'alias' => 'n',
				)
			),
			MigrateDestinationNode::getKeySchema()
		);
		//step 5. map the old fields to the new one
		$this->addFieldMapping('title','title');
		$this->addFieldMapping('uid','uid')
			->sourceMigration('User');
		$this->addFieldMapping('created','postdate');
		$this->addFieldMapping('changed','postdate');
		$this->addFieldMapping('status','published');
		$this->addFieldMapping('promote')->defaultValue(0);
		$this->addFieldMapping('sticky')->defaultValue(0);
		$this->addFieldMapping('is_new')->defaultValue(0);
		$this->addFieldMapping('body','body');
		$this->addFieldMapping('field_date','startdate');
		/*
		
		
		$this->addFieldMapping('');
		$this->addFieldMapping('');
		$this->addFieldMapping('');
		 */
		//skip these source fields
		$this->addUnmigratedSources(
			array(
				'enddate',
				'term',
				'locationname',
				'username',
			)
		);
		//skip these destination fields
		$this->addUnmigratedDestinations(
			array(
			
				//'promote',
				//'sticky',
				'revision',
				'log',
				'language',
				'tnid',
				'translate',
				'revision_uid',
				'body:summary',
				'body:format',
				'body:language',
				'path',
				'comment',
				'pathauto',
				
				
			)
		);
		
    
    
	}



}