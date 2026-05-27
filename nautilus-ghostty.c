#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include <nautilus-extension.h>

static const char MENU_LABEL[] = "Open in Terminal";

typedef struct { GObject parent_instance; } NautilusGhostty;
typedef struct { GObjectClass parent_class; } NautilusGhosttyClass;

G_DEFINE_TYPE(NautilusGhostty, nautilus_ghostty, G_TYPE_OBJECT);

static void
menu_activate_cb (NautilusMenuItem *item, gpointer user_data)
{
    const char *uri = (const char *) user_data;
    if (!uri) return;

    g_autofree char *path = g_filename_from_uri(uri, NULL, NULL);
    if (!path) return;

    g_autofree char *wd_arg = g_strdup_printf("--working-directory=%s", path);
    const char *argv[] = { "ghostty", wd_arg, NULL };
    g_autoptr(GError) error = NULL;
    if (!g_spawn_async(NULL, (char **)argv, NULL,
                       G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
        g_warning("Failed to launch Ghostty: %s", error ? error->message : "(unknown)");
    }
}

static GList *
menu_add_ghostty (const char *name, NautilusFileInfo *info)
{
    if (!name || !info) return NULL;

    g_autofree char *uri_scheme = nautilus_file_info_get_uri_scheme(info);
    if (g_strcmp0(uri_scheme, "file") != 0 || !nautilus_file_info_is_directory(info))
        return NULL;

    char *uri = nautilus_file_info_get_uri(info);
    if (!uri) return NULL;

    NautilusMenuItem *item = nautilus_menu_item_new(name, MENU_LABEL, NULL, NULL);
    g_signal_connect_data(item, "activate", G_CALLBACK(menu_activate_cb),
                          uri, (GClosureNotify) g_free, G_CONNECT_DEFAULT);
    return g_list_append(NULL, item);
}

static GList *
nautilus_ghostty_get_file_items (NautilusMenuProvider *provider, GList *files)
{
    if (!files || g_list_next(files) != NULL) return NULL;
    return menu_add_ghostty("NautilusGhostty::OpenDirectoryInGhostty",
                            NAUTILUS_FILE_INFO(files->data));
}

static GList *
nautilus_ghostty_get_background_items (NautilusMenuProvider *provider,
                                       NautilusFileInfo *current_folder)
{
    if (!current_folder) return NULL;
    return menu_add_ghostty("NautilusGhostty::OpenBackgroundInGhostty", current_folder);
}

static void
nautilus_ghostty_menu_provider_interface_init (NautilusMenuProviderInterface *iface)
{
    iface->get_file_items = nautilus_ghostty_get_file_items;
    iface->get_background_items = nautilus_ghostty_get_background_items;
}

static void nautilus_ghostty_init (NautilusGhostty *self) {}
static void nautilus_ghostty_class_init (NautilusGhosttyClass *klass) {}

void
nautilus_module_initialize (GTypeModule *module)
{
    nautilus_ghostty_get_type();
    static const GInterfaceInfo iface_info = {
        .interface_init = (GInterfaceInitFunc) nautilus_ghostty_menu_provider_interface_init,
    };
    g_type_module_add_interface(module, nautilus_ghostty_get_type(),
                                NAUTILUS_TYPE_MENU_PROVIDER, &iface_info);
}

void nautilus_module_shutdown (void) {}

void
nautilus_module_list_types (const GType **types, int *num_types)
{
    static GType type_list[1] = { 0 };
    type_list[0] = nautilus_ghostty_get_type();
    *types = type_list;
    *num_types = 1;
}
