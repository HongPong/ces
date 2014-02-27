Community Exchange System Drupal Module Suite

CES is a set of modules to deploy a community exchange network using social
currency (also known as complementary currency or local currency). 

This module wants to be a new version of the popular software Community Exchange
System ( http://www.ces.org.za ) but it is not officially supported by
ces.org.za nor by its authors.

=======

Development sandbox repository:
http://drupal.org/sandbox/esteve/1367140

Bug tracker:
http://drupal.org/project/issues/1367140?status=All&categories=All

=======
Installation:

1) Install and enable this module.
2) Setup the initial page to /ces
3) Setup blocks.

=======

Module structure:

There is a main virtual module called ces. This module does not have any
feature. It is used as a container for the other modules and contains several
general purpose files.

From ces there hang several modules. Currently there is one main module, the
bank one and also an auxiliar module notify. bank contains the banking
functionality. It basically implements exchange groups, bank accounts and
transactions between bank accounts. notify is a little framework for mailing.
It handles customization and localization of messages.

=======

Developer documentation:

Architecture and file structure

The root folder contains several files and a folder for each submodule.

ces.info and ces.module: These are the mandatory files for the virtual module
ces.
common.db.logic.inc: This file contains a set of classes that acts as a layer
between the CES logic and the Drupal database layer. It basically implements a
lightweight version of the active record pattern. It defines an abstract class
DBObject. Every object that must be loaded/saved to the database must extend
this class and (optionally) override some of its methods. It also defines the
class Serializer responsible for saving and loading DBObjects.
bank: The folder that contains the bank module.
ces_notify: The folder that contains the notify module.
The bank module is structured in the following way that may be established as a
guideline for further CES submodules.

bank.module: It exclusively has Drupal core hook implementations as well as
other hooks exposed by the bank module itself.
bank.install: Standard .install file.
bank.logic.inc: It contains all the logic layer of the bank module. It is
exclusively object oriented and it is Drupal independent. Consists of a set of
PHP classes: Exchange, Account, Transaction, etc. There is a special class,
Bank, which is a controller of the whole logic layer contained in this file.
It is the only interface between the GUI and logic layer. From outside this
file, the Bank class should be the only used class.
bank.forms.inc: It has the definition of the Drupal forms for the bank module.
For instance, a _submit function in this file will call an action to the Bank
class.
bank.pages.inc: It defines the Drupal pages for this module.
bank.test: Some functional tests using the Drupal test framework.
The upshot is: in the .module file we set up the URLs, pointing to forms defined
in a separate file. These forms use the Bank interface to communicate with the
logic. The Bank interface calls the logic layer objects and then updates the DB
using the Serializer class from commons.db.logic.inc file.

The notify module has similar code organization, without forms nor pages. As
bank has the Bank class being its public inteface, notify has the Notifier
class.

=======
Links: 

Community Exchange Systems Wikipedia:
http://en.wikipedia.org/wiki/Community_Exchange_System
