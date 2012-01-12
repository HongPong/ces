<?php
/**
 * @file 
 * Abstraction layer between CES logic and the Drupal mailing system.
 */
/**
 * Every logical message has a unique name. For example the name 'account activated'
 * is related to the message to be sent when an account has just been activated.
 * However, the actual message text depends on several additional data:
 *   1 - The recipient's preferred language.
 *   2 - The available languages in sender's customized messages.
 * 
 * It is highly recommended that each sender has customized messages for at least
 * its own preferred language. It is mandatory for modules using this messaging
 * framework to provide default message templates at least in english.
 */

/**
 * This class is the public interface for this module. 
 */
class Notifier{
  
  /**
   * Creates or updates a message template.
   * 
   * @param $record array with the following (mandatory) entries:
   *  - 'module': message name.
   *  - 'name': message name.
   *  - 'lang': message lang code.
   *  - 'sender': sender id
   *  - 'subject': message subject
   *  - 'body': message body
   */
  public function updateMessageTemplate($record){
    $s = new Serializer('Message');
    $message = $s->loadFromRecord($record);
    
    if(($oldMessage = $this->loadMessageTemplate($record['module'], $record['name'], $record['lang'], $record['sender']))!==FALSE){
      //message alredy exists
      $message->id = $oldMessage->id;
    }
    $s->save($message);
  }
  
  /**
   * Sets the subject and body and the language of a message.
   * 
   * The most appropiate template is choosen to be the one that first meets one 
   * of the following conditions:
   * 1. Sender's template exists with language = $record['lang']
   * 2. Sender's template exists with language = user_preferred_language(recipient)
   * 3. Sender's template exists with language = user_preferred_language(sender)
   * 4. Default template exists with language = $record['lang']
   * 5. Default template exists with language = user_preferred_language(recipient)
   * 6. Default template exists with language = user_preferred_language(sender)
   * 7. Default template exists with language = language_default()
   * 8. Default template exists with language = en
   * 
   * @param $record array 
   * Input and output associative array with entries:
   *  - 'module': Input. The module name defining the key.
   *  - 'name': Input. The message key.
   *  - 'sender: Optional Input. The sender's uid.
   *  - 'recipient': Optional Input. The recipient's uid.
   *  - 'lang': Optional input/Output. The desired language/The actual output language.
   *  - 'subject': Output. The subject of the message.
   *  - 'body': Output. The body of the message.
   */
  
  public function getMessageTemplate(&$record){
    $module = $record['module'];
    $name = $record['name'];
    $sender = isset($record['sender'])?$record['sender']:1;
    
    if(isset($record['lang'])){//1
      $message = $this->loadMessageTemplate($module, $name, $record['lang'], $sender);
    }
    if(empty($message) && isset($record['recipient'])){//2
      $recipientlang = user_load($record['recipient'])->language;
      $message = $this->loadMessageTemplate($module, $name, $recipientlang, $sender);
    }
    if(empty($message) && isset($record['sender'])){//3
      $senderlang = user_load($record['sender'])->language;
      $message = $this->loadMessageTemplate($module, $name, $senderlang, $sender);
    }
    
    if(!empty($message)){
    
      $record['subject'] = $message->subject;
      $record['body'] = $message->body;
      $record['lang'] = $message->lang;
    
    }else{//customized message does not exist. Try module translations
    
      $default_eng = module_invoke($module, 'ces_notify_messages', $name, 'en');
      if(empty($default_eng)){
        throw new Exception(t('Module %module does not implement key %key in hook_ces_notify_messages',array('%module'=>$module, '%key'=>$name)));
      }
      $default_eng['lang'] = 'en';
      
      if(isset($record['lang'])){
        $default = $this->loadDefaultTemplateHelper($module, $name, $record['lang'], $default_eng);
      }
      if(empty($default) && isset($recipientlang)){
        $default = $this->loadDefaultTemplateHelper($module, $name, $recipientlang, $default_eng);
      }
      if(empty($default) && isset($senderlang)){
        $default = $this->loadDefaultTemplateHelper($module, $name, $senderlang, $default_eng);
      }
      if(empty($default) && language_default('language')!='en'){
        $default = $this->loadDefaultTemplateHelper($module, $name, language_default('language'), $default_eng);
      }
      if(empty($default)){
        $default = &$default_eng;
      }
      
      $record['lang'] = $default['lang'];
      $record['subject'] = $default['subject'];
      $record['body'] = $default['body'];
    }
  }
  /**
   * @return array
   */
  private function loadDefaultTemplateHelper($module, $name, $lang, $default_eng){
    if($lang == 'en'){
      $res = $default_eng;
    }else{
      $default_aux = module_invoke($module, 'ces_notify_messages', $name, $lang);
      if($default_aux != $default_eng){//depth array comparison
        $res = $default_aux;
        $res['lang'] = $lang;
      }else{
        return FALSE;
      }
    }
    return $res;
  }
  
  /**
   * @return Message or FALSE if template does not exist.
   */
  private function loadMessageTemplate($module, $name, $lang, $sender){
    $conditions = array(
      'module' => $module,
      'name' => $name,
      'lang' => $lang,
      'sender' => $sender,
    );
    $s = new Serializer('Message');
    $records = $s->loadCollection($conditions);//it has zero or one result
    if(!empty($records)) 
      return reset($records);
    else
      return FALSE;
  }
  /**
   * Senda an e-mail to a user.
   * @param $key string the message key
   * @param $recipientId int the recipient user id
   * @param $senderId int the sender user id
   * @param $params array Associative array with the placeholder values for this 
   * message
   */
  public function notify($module, $key, $recipientId, $senderId, array $params){
    $recipient = user_load($recipientId);
    $sender = user_load($senderId);
    drupal_mail('ces_notify', $module . ' ' . $key, $recipient->mail, NULL, $params, $sender->mail, TRUE);
  }
}

/**
 * This class is the persistent object that represents a message template.
 */
class Message extends DBObject{
  public static $table = 'ces_messages';
  /**
   * @var int
   */
  public $id;
  /**
   * @var string
   */
  public $name;
  /**
   * @var string
   */
  public $lang;
  /**
   * @var int
   */
  public $sender;
  /**
   * @var string
   */
  public $subject;
  /**
   * @var string
   */
  public $body;
  
}
