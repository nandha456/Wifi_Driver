#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ieee80211.h>
#include <net/cfg80211.h>
#include <net/regulatory.h>

// 1. The Regulatory Callback (belongs to struct wiphy, not cfg80211_ops)
// This function is called when the kernel wants to know which country's Wi-Fi rules to apply.
static void my_wifi_reg_notifier(struct wiphy *wiphy,
                                struct regulatory_request *request) {
    printk(KERN_INFO "my_wifi: Regulatory request received (dummy)\n");
    // In a real driver, we would apply country-specific frequency rules here.
}

// 2. The Operations Structure
static const struct cfg80211_ops my_wifi_ops = {
};

static struct ieee80211_channel my_wifi_channels[] = {
    {
        .band = NL80211_BAND_2GHZ,
        .center_freq = 2412,
        .hw_value = 1,
        .flags = IEEE80211_CHAN_NO_HT40,
    },
};

static struct ieee80211_supported_band my_wifi_band_2ghz = {
    .band = NL80211_BAND_2GHZ,
    .n_channels = ARRAY_SIZE(my_wifi_channels),
    .channels = my_wifi_channels,
    .n_bitrates = 0,
    .bitrates = NULL,
};

// 3. Global pointer to our Wi-Fi PHY
static struct wiphy *g_wiphy;

// 4. Module Init
static int __init my_wifi_init(void) {
    int ret;
    printk(KERN_INFO "my_wifi: Initializing Wi-Fi PHY\n");

    // Allocate a new wiphy
    g_wiphy = wiphy_new(&my_wifi_ops, 0);
    if (!g_wiphy)
        return -ENOMEM;

    g_wiphy->reg_notifier = my_wifi_reg_notifier;
    g_wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION) |
                               BIT(NL80211_IFTYPE_AP);
    g_wiphy->bands[NL80211_BAND_2GHZ] = &my_wifi_band_2ghz;
    g_wiphy->max_scan_ssids = 4;
    g_wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;

    ret = wiphy_register(g_wiphy);
    if (ret) {
        printk(KERN_ERR "my_wifi: Failed to register wiphy (error %d)\n", ret);
        wiphy_free(g_wiphy);
        return ret;
    }

    printk(KERN_INFO "my_wifi: Wi-Fi PHY registered successfully as %s!\n",
           wiphy_name(g_wiphy));
    return 0;
}

// 5. Module Exit
static void __exit my_wifi_exit(void) {
    printk(KERN_INFO "my_wifi: Cleaning up Wi-Fi PHY\n");
    
    if (g_wiphy) {
        wiphy_unregister(g_wiphy);
        wiphy_free(g_wiphy);
    }
}

module_init(my_wifi_init);
module_exit(my_wifi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fresher Dev");
MODULE_DESCRIPTION("A virtual Wi-Fi PHY using cfg80211 (Kernel 7.0 compatible)");