
#include "mustache_template.hpp"



// Method Entries --------------------------------------------------------------

static zend_function_entry MustacheTemplate_methods[] = {
  //PHP_ME(MustacheTemplate, __construct, MustacheTemplate____construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
  PHP_ME(MustacheTemplate, render, MustacheTemplate__render_args, ZEND_ACC_PUBLIC)
  PHP_ME(MustacheTemplate, toArray, MustacheTemplate__toArray_args, ZEND_ACC_PUBLIC)
  { NULL, NULL, NULL }
};



// Object Handlers -------------------------------------------------------------

static zend_object_handlers MustacheTemplate_obj_handlers;

static void MustacheTemplate_obj_free(void *object TSRMLS_DC)
{
  php_obj_MustacheTemplate *payload = (php_obj_MustacheTemplate *)object;

  mustache::Mustache * mustache = payload->mustache;
  mustache::Node * node = payload->node;

  if( node != NULL ) {
    delete node;
  }
  if( mustache != NULL ) {
    delete mustache;
  }

  efree(object);
}

static zend_object_value MustacheTemplate_obj_create(zend_class_entry *class_type TSRMLS_DC)
{
  php_obj_MustacheTemplate *payload;
  zval *tmp;
  zend_object_value retval;

  payload = (php_obj_MustacheTemplate *)emalloc(sizeof(php_obj_MustacheTemplate));
  memset(payload, 0, sizeof(php_obj_MustacheTemplate));
  payload->obj.ce = class_type;

  payload->mustache = new mustache::Mustache;
  payload->node = new mustache::Node;

  retval.handle = zend_objects_store_put(payload, NULL, (zend_objects_free_object_storage_t) MustacheTemplate_obj_free, NULL TSRMLS_CC);
  retval.handlers = &MustacheTemplate_obj_handlers;

  return retval;
}



// MINIT -----------------------------------------------------------------------

PHP_MINIT_FUNCTION(mustache_template)
{
  zend_class_entry ce;

  INIT_CLASS_ENTRY(ce, "MustacheTemplate", MustacheTemplate_methods);
  ce.create_object = MustacheTemplate_obj_create;
  MustacheTemplate_ce_ptr = zend_register_internal_class(&ce);
  memcpy(&MustacheTemplate_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
  MustacheTemplate_obj_handlers.clone_obj = NULL;
  
  return SUCCESS;
}



// Methods ---------------------------------------------------------------------

/* {{{ proto string render(array data, array partials)
   */
PHP_METHOD(MustacheTemplate, render)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_MustacheTemplate * payload;
  
  zval * data = NULL;
  zval * partials = NULL;
  
  mustache::Data templateData;
  mustache::Node::Partials templatePartials;
  std::string output;
  
  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oa/|a/", &_this_zval, MustacheTemplate_ce_ptr, &data, &partials) == FAILURE) {
          return;
  }
  
  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_MustacheTemplate *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  if( payload->mustache == NULL || payload->node == NULL ) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "MustacheTemplate was not initialized properly");
    RETURN_FALSE;
    return;
  }
  
  try {
    // Prepare template data
    mustache_data_from_zval(&templateData, data);
    
    // Tokenize partials
    mustache_partials_from_zval(payload->mustache, &templatePartials, partials);
    
    // Render template
    payload->mustache->render(payload->node, &templateData, &templatePartials, &output);
    
    // Output
    RETURN_STRING(output.c_str(), 1); // Yes reallocate
    
  } catch( mustache::Exception& e ) {
    
    php_error(E_WARNING, (char *) e.what());
    RETURN_FALSE;
    
  }
}
/* }}} render */

/* {{{ proto array toArray()
   */
PHP_METHOD(MustacheTemplate, toArray)
{
  zend_class_entry * _this_ce;
  zval * _this_zval;
  php_obj_MustacheTemplate * payload;
  
  if( zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &_this_zval, MustacheTemplate_ce_ptr) == FAILURE) {
          return;
  }
  
  _this_zval = getThis();
  _this_ce = Z_OBJCE_P(_this_zval);

  payload = (php_obj_MustacheTemplate *) zend_object_store_get_object(_this_zval TSRMLS_CC);
  
  if( payload->mustache == NULL || payload->node == NULL ) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "MustacheTemplate was not initialized properly");
    RETURN_FALSE;
    return;
  }
  
  // Tokenize template
  try {
    
    // Convert to PHP array
    mustache_node_to_zval(payload->node, return_value);
    
  } catch( mustache::Exception& e ) {
    
    php_error(E_WARNING, (char *) e.what());
    RETURN_FALSE;
    
  }
}
/* }}} toArray */