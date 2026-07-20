#ifndef _RULES_H_
#define _RULES_H_

#include <items_ext.h>

//   ----------------------------------  replace  -----------------------------------------
// D
bool rule_replace_decimal(Item** pItem);
// I
bool rule_replace_I(Item** pItem);
// R-R
bool rule_replace_sub(Item** pItem);
// (+R) or (-R)
bool rule_replace_sign(Item** pItem);
// R/R
bool rule_replace_frac(Item** pItem);
// R^R
bool rule_replace_power(Item** pItem);

//   ----------------------------------  basic  --------------------------------------------
// R+R
bool rule_add_R(Item** pItem);
// R*R
bool rule_mult_R(Item** pItem);
// R!
bool rule_factorial_R(Item** pItem);
// Root(R;R)
bool rule_root_R(Item** pItem);




#endif /* _RULES_H_ */

