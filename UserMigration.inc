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
    
    //map table
    $this->map=new MigrateSQLMap($this->machineName,
    	array(
    		'unr' =>array(
    			'type' => 'varchar',
    			'length' => 8,
    			'not null' => TRUE,
    			'description' => 'The KU Leuven unumber',
    		),	
    	),
    	MigrateDestinationUser::getKeySchema()
    );
    
    
    
    //SQL Source
	//step 1. Define external Database
	//from settings.php
	/*
	$db_url = 'mysql://alegria:my08ens1al@localhost/alegria';
	$db_prefix = 'drupal_';
	*/
	Database::addConnectionInfo('for_migration', 'default', array(
		'driver' => 'mysql',
		'database' => 'alegria',
		'username' => 'alegria',
		'password' => 'my08ens1al',
		'host' => 'http://mysql.webnet4u.be',
		'prefix' => 'drupal_',
    ));
	//step 2. Query the database
	$query = Database::getConnection('default', 'for_migration')
           ->select('users', 'u')
    
    //destination
    $this->destination = new MigrateDestinationUser();
    
    //mapping
    $this->addFieldMapping('name','unr');
    $this->addFieldMapping('mail','email');
    //$this->addFieldMapping('role_names','roles')
    //		 ->separator(',');
    
     //skip these source fields
    $this->addUnmigratedSources(
    	array(
    		'name',
    		'course',
    		'monitor',
    		'id',
    		'roles',
    	)
    );
    //skip these destination fields
    $this->addUnmigratedDestinations(
    	array(
    		'is_new',
    		'pass',
    		'roles',
    		'theme',
    		'signature',
    		'signature_format',
    		'created',
    		'access',
    		'login',
    		'status',
    		'timezone',
    		'language',
    		'picture',
    		'init',
    		'path',
    		'pathauto',
    	)
    );
    
    //add dependency
    //$this->dependencies = array('UserRole');
    
	}

	//it is executing before saving migrated object to the destination
  public function prepare($user,stdClass $row){
  	//add all the role ids to the user account
  	$role_names = explode(',',$row->roles);
  	
  	//check if role didactisch team bestaat
  	$new_role = array('didactisch team');
  	foreach($new_role as $r){
  		if(!user_role_load_by_name($r)){
  			//create it
  			$role = new stdClass();
  			$role->name = $r;
  			user_role_save($role);
  		}
  	}
  	$role = user_role_load_by_name('didactisch team');
 		//$user role is a mapping from role id to role name
 		$user->roles[$role->rid] = $role->name;
  	//drupal_set_message(print_r($user,true),'warning');
  	//set the roles
  	//$user->roles = $role_ids;
  }

}