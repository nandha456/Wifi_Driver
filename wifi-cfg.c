#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <net/cfg80211.h>

// ==========================================
// 1. DUMMY HARDWARE SPECIFICATIONS
// ==========================================

// Define a dummy 2.4GHz Channel (Channel 1, 2412 MHz)
static struct ieee80211_channel my_wifi_channels[] = {
    {
        .band = NL80211_BAND_2GHZ,
        .center_freq = 2412, // 2412 MHz is Wi-Fi Channel 1
        .hw_value = 1,
        .max_power = 30,     // 30 dBm transmit power
    }
};

// Define dummy data rates (1 Mbps and 2 Mbps)
// bitrate is in units of 100 kbps (so 10 = 1 Mbps)
static struct ieee80211_rate my_wifi_rates[] = {
    { .bitrate = 10, .hw_value = 1, }, 
    { .bitrate = 20, .hw_value = 2, }, 
};

// Tie the channels and rates together into a "Supported Band"
static struct ieee80211_supported_band my_wifi_sband_2ghz = {
    .band = NL80211_BAND_2GHZ,
    .channels = my_wifi_channels,
    .n_channels = ARRAY_SIZE(my_wifi_channels),
    .bitrates = my_wifi_rates,
    .n_bitrates = ARRAY_SIZE(my_wifi_rates),
};

// ==========================================
// 2. CFG80211 OPERATIONS
// ==========================================

// In your modern kernel (7.1.x), get_reg was removed.
// We leave this empty for now. The kernel will just return "Not Supported"
// if user-space tries to do complex things, which is fine for our virtual driver!
static const struct cfg80211_ops my_wifi_ops = {
    // Empty for now!
};

// ==========================================
// 3. MODULE INIT & EXIT
// ==========================================

static struct wiphy *g_wiphy;

static int __init my_wifi_init(void) {
    int ret;
    printk(KERN_INFO "my_wifi: Initializing Virtual Wi-Fi PHY\n");

    // Allocate wiphy
    g_wiphy = wiphy_new(&my_wifi_ops, 0);
    if (!g_wiphy) {
        printk(KERN_ERR "my_wifi: Failed to allocate wiphy\n");
        return -ENOMEM;
    }

    // Tell the kernel what kind of interfaces we support (Station/Client mode)
    g_wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION);

    // Assign our dummy 2.4GHz band to the wiphy
    g_wiphy->bands[NL80211_BAND_2GHZ] = &my_wifi_sband_2ghz;

    // Register the wiphy
    ret = wiphy_register(g_wiphy);
    if (ret) {
        printk(KERN_ERR "my_wifi: Failed to register wiphy (error %d)\n", ret);
        wiphy_free(g_wiphy);
        return ret;
    }

    printk(KERN_INFO "my_wifi: Wi-Fi PHY registered successfully as %s!\n", wiphy_name(g_wiphy));
    return 0;
}

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
MODULE_DESCRIPTION("Virtual Wi-Fi PHY with dummy 2.4GHz band");