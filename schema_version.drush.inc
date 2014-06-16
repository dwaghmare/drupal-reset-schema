<?php

/**
 * Implements hook_drush_command().
 */
function schema_version_drush_command() {
  $items = array();

  $items['schema-version-get'] = array(
    'description' => 'Get schema version',
    'arguments' => array(
      'module' => 'The module name.',
    ),
    'required-arguments' => TRUE,
    'aliases' => array('svg'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_DATABASE,
  );
  $items['schema-version-set'] = array(
    'description' => 'Set schema version',
    'arguments' => array(
      'module' => 'The module name.',
      'schema-version' => 'A schema version to set the module to.',
    ),
    'required-arguments' => TRUE,
    'aliases' => array('svs'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_DATABASE,
  );

  return $items;
}

function drush_schema_version_get($module) {
  // Ensure that install.inc will be included.
  include_once DRUSH_DRUPAL_CORE . '/includes/install.inc';

  $version = drupal_get_installed_schema_version($module);
  if ($version > SCHEMA_UNINSTALLED) {
    drush_print(dt("Module @module is currently at schema version $version.", array('@module' => $module)));
  }
  else {
    drush_log(dt("Module @module is not installed.", array('@module' => $module)), 'error');
    return FALSE;
  }
}

function drush_schema_version_set($module, $version) {
  // Ensure that install.inc will be included.
  include_once DRUSH_DRUPAL_CORE . '/includes/install.inc';

  $current_version = drupal_get_installed_schema_version($module);
  if ($current_version == SCHEMA_UNINSTALLED) {
    drush_log(dt("Module @module is not installed.", array('@module' => $module)), 'error');
    return FALSE;
  }

  if (preg_match('/current([\+\-])(\d+)/', $version, $matches)) {
    switch ($matches[1]) {
      case '+':
        $version = $current_version + (int) $matches[2];
        break;
      case '-':
        $version = $current_version - (int) $matches[2];
        break;
    }
  }

  $version = (int) $version;
  drupal_set_installed_schema_version($module, $version);
  $new_version = drupal_get_installed_schema_version($module);
  if ($new_version == $version) {
    drush_log(dt("Updated module @module to schema version @version.", array('@module' => $module, '@version' => $version)), 'success');
  }
  else {
    drush_log(dt("Unable to set module @module schema to @version.", array('@module' => $module, '@version' => $version)), 'error');
  }
}
