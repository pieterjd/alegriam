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
		
		//dependency on user migration
		//$this->dependencies = array('User');
		
		//blogpost of using migrate module from D6 to D7
		//http://dtek.net/blog/drupal-6-to-drupal-7-via-migrate-2
		
		//SQL Source
		//step 1. Define external Database
		//cf https://drupal.org/node/1014558
		//from settings.php of old system
		
		//$db_url = 'mysql://alegria:my08ens1al@localhost/alegria';
		//$db_prefix = 'drupal_';
		//IMPORTANT: REMOVE http:// from host string!!!!
		
		
		
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
		//$this->addFieldMapping('is_new')->defaultValue(0);
		$this->addFieldMapping('body','body')
			->arguments(
				array(
					'format' => 'filtered_html',
				)
			);
		//next line does not work
		//$this->addFieldMapping('field_date','startdate');
		// check https://drupal.org/node/1806296#comment-6589830 for info
		//datespan is a dummy field on the source side
		$this->addFieldMapping( 'field_date', 'datespan' );
		
		$this->addFieldMapping('field_text_location','LocationName');
		
		$this->addFieldMapping('field_event_term','term');
		$this->addFieldMapping('field_event_term:create_term')
			->defaultValue(TRUE);
		/*
		
		
		$this->addFieldMapping('');
		$this->addFieldMapping('');
		$this->addFieldMapping('');
		 */
		//skip these source fields
		$this->addUnmigratedSources(
			array(
				'startdate',
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
				'field_text_location:language',
				'path',
				'comment',
				'pathauto',
				'is_new',
				'field_event_term:source_type',
				'field_event_term:ignore_case',
				'field_location',
				
				
			)
		);
		
    
    
	}
	//as suggested in https://drupal.org/node/1806296#comment-6589830
	public function prepareRow($row) {
		//for some reason, 2 hours have to be added
		$row->startdate = strtotime("+2 hours",$row->startdate);
		$row->enddate = strtotime("+2 hours",$row->enddate);
		$row->datespan = drupal_json_encode(array(
			'from' 	=> $row->startdate,
			'to' 	=> $row->enddate
		));
		//change linebreaks back to newlines
		$row->body = str_replace('<br/>',"\n",$row->body);
		
		
	}
	
	public function prepare($entity,stdClass $row){
		//try to add location
		$keywords = array('Claes','Hand','Roose','Kerk','Torenhof','harmonie');
		foreach($keywords as $key){
			if(stristr($row->locationname,$key)){
				//found $key in locationname of current row
				// dsm("found $key in locationname");
				//now look for corresponding location node on destination side
				$loc_query = db_select('node','n')
					->fields('n')
					->condition('n.type','location')
					->condition('n.title','%'.$key.'%','LIKE'); //MYSQL's like is case insensitive
				// dpm((string)$loc_query);
				// dpm($loc_query->arguments());
					
				$locations = $loc_query->execute();
				
				if(!empty($locations)){
					// dsm('location found!');
					// dpm($locations);
					dpm($entity);
					foreach($locations as $loc){
						//target_id is specific for entity reference
						// cf https://drupal.org/node/1845986
						$entity->field_location['und'][0]['target_id'] = $loc->nid;
					}
					dpm($entity);
					// $locations = node_load(array_shift(array_keys($locations['node'])));
					// dsm($locations);
					// $entity->field_location = $locations;
				}
				
			}
			
		}
	}
	

}