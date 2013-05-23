<?php
/**
 * @file template for offer want posts.
 */
?>
<div class="ces-offerwant">
  <div class="ces-offerwant-display" >
    <div class="ces-offerwant-title"><?php echo $ces_offerwant_title; ?></div>
    <div class="ces-offerwant-content">
      <div class="ces-offerwant-image"><?php 
      if (!empty($ces_offerwant_image)):
        $ces_offerwant_image['width'] = '200px';
        echo theme('image', $ces_offerwant_image); 
      endif;
      ?></div>
      <div class="ces-offerwant-body"><?php echo $ces_offerwant_body; ?></div>
      <div class="ces-offerwant-rate"><?php echo $ces_offer_rate ?></div>
    </div>
  </div>
  <?php if ($view_mode_full): ?>
  <div class="ces-offerwant-properties">
    <dl class="ces-offerwant-properties-list">
      <dt><?php echo t('Category'); ?></dt>
      <dd><?php echo $ces_offerwant_category; ?></dd>
      <dt><?php echo t('Status'); ?></dt>
      <dd><?php echo $ces_offerwant_state; ?></dd>
      <dt><?php echo t('Updated'); ?></dt>
      <dd><?php echo $ces_offerwant_modified; ?></dd>
      <dt><?php echo t('Expire'); ?></dt>
      <dd><?php echo $ces_offerwant_expire; ?></dd>
      <dt><?php echo t('Keywords'); ?></dt>
      <dd><?php echo $ces_offerwant_keywords; ?></dd>
    </dl>
  </div>
  <?php endif; ?>
  <div class="ces-offerwant-footer">
    <div class="ces-offerwant-seller">
      <?php global $base_path; ?>
      <div class="ces-offerwant-seller-picture">
        <a href="<?php print $base_path; ?>user/<?php echo $ces_offerwant_seller_uid; ?>"  title="<?php echo t('Seller\'s info'); ?>">
      <?php print theme('user_picture', array('account' => user_load($ces_offerwant_seller_uid))); ?>
        </a>
      </div>
      <div class="ces-offerwant-seller-name-and-phone">
        <div class="ces-offerwant-seller-name">
          <a href="<?php print $base_path; ?>user/<?php echo $ces_offerwant_seller_uid; ?>" title="<?php echo t('Seller\'s info'); ?>">
          <?php echo $ces_offerwant_seller_name; ?>
          </a>
        </div>
        <div class="ces-offerwant-seller-phone">
            <?php echo $ces_offerwant_seller_phone; ?>
        </div>
        <?php if( $view_mode_full && $ces_offerwant_belongs_to_me == false ) : ?>
            <div class="ces-offerwant-seller-email">
            <?php echo $ces_offerwant_seller_mail; ?>
            </div>
            <div class="ces-offerwant-seller-address">
            <?php echo $ces_offerwant_seller_address; ?>
            </div>
        <?php endif; ?>
      </div>
    </div>
    <div class="ces-offerwant-actions">
      <?php echo $ces_offerwant_actions; ?>
    </div>
    <div class="clearfix"></div>
  </div>
  
</div>
