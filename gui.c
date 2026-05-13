#include <gtk/gtk.h>
#include <stdio.h>
#include "Integration.h"

IntegrationSystem app;
GtkListStore *inventory_store;
GtkWidget *inventory_view;
GtkListStore *customer_store;
GtkWidget *customer_view;
GtkListStore *order_store;
GtkWidget *order_view;
GtkListStore *admin_store;
GtkWidget *admin_view;
GtkWidget *statusbar;
int inventory_row = 0;

// Button callback prototypes
void on_notebook_switch_page(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer data);
void update_statusbar();
void on_main_window_destroy(GtkWidget *widget, gpointer data);
void on_btn_add_product_clicked(GtkButton *button, gpointer data);
void on_btn_delete_product_clicked(GtkButton *button, gpointer data);
void on_btn_update_stock_clicked(GtkButton *button, gpointer data);
void on_btn_search_product_clicked(GtkButton *button, gpointer data);
void on_btn_add_customer_clicked(GtkButton *button, gpointer data);
void on_btn_delete_customer_clicked(GtkButton *button, gpointer data);
void on_btn_search_customer_clicked(GtkButton *button, gpointer data);
void on_btn_place_order_clicked(GtkButton *button, gpointer data);
void on_btn_process_order_clicked(GtkButton *button, gpointer data);
void on_btn_undo_clicked(GtkButton *button, gpointer data);
void on_btn_view_log_clicked(GtkButton *button, gpointer data);

void setup_inventory_treeview(GtkWidget *treeview) {
    inventory_store = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING);
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "ID",    renderer, "text", 0, "background", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Name",  renderer, "text", 1, "background", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Price", renderer, "text", 2, "background", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Stock", renderer, "text", 3, "background", 4, NULL);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(inventory_store));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    int widths[] = {70, 350, 100, 80};
    GList *columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview));
    int i = 0;
    for (GList *col = columns; col != NULL; col = col->next, i++) {
        gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(col->data), widths[i]);
        gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(col->data), GTK_TREE_VIEW_COLUMN_FIXED);
    }
    g_list_free(columns);
}

void populate_inventory(Product *root) {
    if (root == NULL) return;
    populate_inventory(root->left);
    GtkTreeIter iter;
    char price_str[20];
    snprintf(price_str, sizeof(price_str), "%.2f", root->price);
    const char *bg = (inventory_row++ % 2 == 0) ? "#ffffff" : "#eef4fb";
    gtk_list_store_append(inventory_store, &iter);
    gtk_list_store_set(inventory_store, &iter,
        0, root->productID,
        1, root->name,
        2, price_str,
        3, root->stock,
        4, bg,
        -1);
    populate_inventory(root->right);
}

void setup_customer_treeview(GtkWidget *treeview) {
    customer_store = gtk_list_store_new(4, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "ID",    renderer, "text", 0, "background", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Name",  renderer, "text", 1, "background", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Phone", renderer, "text", 2, "background", 3, NULL);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(customer_store));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    int widths[] = {80, 200, 150};
    GList *columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview));
    int i = 0;
    for (GList *col = columns; col != NULL; col = col->next, i++) {
        gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(col->data), widths[i]);
        gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(col->data), GTK_TREE_VIEW_COLUMN_FIXED);
    }
    g_list_free(columns);
}

void populate_customers() {
    gtk_list_store_clear(customer_store);
    int row = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        Customer *current = app.customerTable[i];
        while (current != NULL) {
            GtkTreeIter iter;
            const char *bg = (row++ % 2 == 0) ? "#ffffff" : "#eef4fb";
            gtk_list_store_append(customer_store, &iter);
            gtk_list_store_set(customer_store, &iter,
                0, current->CustomerID,
                1, current->Name,
                2, current->Phone,
                3, bg,
                -1);
            current = current->next;
        }
    }
}

void setup_order_treeview(GtkWidget *treeview) {
    order_store = gtk_list_store_new(7, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Order ID",      renderer, "text", 0, "background", 6, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Customer ID",   renderer, "text", 1, "background", 6, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Product ID",    renderer, "text", 2, "background", 6, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Quantity",      renderer, "text", 3, "background", 6, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Shipping Type", renderer, "text", 4, "background", 6, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Status",        renderer, "text", 5, "background", 6, NULL);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(order_store));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    int widths[] = {90, 110, 100, 80, 130, 100};
    GList *columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview));
    int i = 0;
    for (GList *col = columns; col != NULL; col = col->next, i++) {
        gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(col->data), widths[i]);
        gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(col->data), GTK_TREE_VIEW_COLUMN_FIXED);
    }
    g_list_free(columns);
}

void populate_orders() {
    gtk_list_store_clear(order_store);
    int row = 0;
    Order *current = app.orderFront;
    while (current != NULL) {
        GtkTreeIter iter;
        const char *bg = (row++ % 2 == 0) ? "#ffffff" : "#eef4fb";
        gtk_list_store_append(order_store, &iter);
        gtk_list_store_set(order_store, &iter,
            0, current->orderID,
            1, current->customerID,
            2, current->productID,
            3, current->quantity,
            4, current->shippingType,
            5, current->status,
            6, bg,
            -1);
        current = current->next;
    }
}

void setup_admin_treeview(GtkWidget *treeview) {
    admin_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Action",      renderer, "text", 0, "background", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Product ID",  renderer, "text", 1, "background", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview), -1, "Description", renderer, "text", 2, "background", 3, NULL);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(admin_store));
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    int widths[] = {150, 100, 400};
    GList *columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview));
    int i = 0;
    for (GList *col = columns; col != NULL; col = col->next, i++) {
        gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(col->data), widths[i]);
        gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(col->data), GTK_TREE_VIEW_COLUMN_FIXED);
    }
    g_list_free(columns);
}

void populate_admin_log() {
    gtk_list_store_clear(admin_store);
    FILE *fp = fopen(ADMIN_LOG_FILE, "r");
    if (!fp) return;
    char line[256];
    int row = 0;
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        char action[20], desc[200];
        int id;
        if (sscanf(line, "%[^,],%d,%[^\n]", action, &id, desc) == 3) {
            GtkTreeIter iter;
            const char *bg = (row++ % 2 == 0) ? "#ffffff" : "#eef4fb";
            gtk_list_store_append(admin_store, &iter);
            gtk_list_store_set(admin_store, &iter, 0, action, 1, id, 2, desc, 3, bg, -1);
        }
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // 1. Global CSS
    GtkCssProvider *css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css,
        "window { background-color: #f0f4f8; }"
        "notebook tab { padding: 8px 16px; font-size: 11pt; font-weight: bold; }"
        "notebook tab:checked { background-color: #ffffff; border-bottom: 2px solid #2196F3; }"
        "treeview { font-size: 11pt; color: #1a1a1a; background-color: #ffffff; }"
        "treeview:selected { background-color: #2196F3; color: #ffffff; }"
        "treeview header button { font-weight: bold; background-color: #dce8f5; color: #1a3a5c; padding: 6px; }"
        "button { padding: 8px 16px; font-size: 10pt; border-radius: 6px; font-weight: bold; }"
        "button:hover { opacity: 0.85; }"
        "dialog label { font-size: 10pt; margin: 4px 8px; }"
        "entry { padding: 4px; font-size: 10pt; margin: 4px 8px; }"
        , -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    // 2. Load UI
    GtkBuilder *builder = gtk_builder_new_from_file("main.ui");
    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    GtkWidget *notebook = GTK_WIDGET(gtk_builder_get_object(builder, "main_notebook"));
    g_signal_connect(notebook, "switch-page", G_CALLBACK(on_notebook_switch_page), NULL);

    // 3. Get all buttons
    GtkWidget *btn_add             = GTK_WIDGET(gtk_builder_get_object(builder, "btn_add_product"));
    GtkWidget *btn_delete          = GTK_WIDGET(gtk_builder_get_object(builder, "btn_delete_product"));
    GtkWidget *btn_update          = GTK_WIDGET(gtk_builder_get_object(builder, "btn_update_stock"));
    GtkWidget *btn_search          = GTK_WIDGET(gtk_builder_get_object(builder, "btn_search_product"));
    GtkWidget *btn_add_customer    = GTK_WIDGET(gtk_builder_get_object(builder, "btn_add_customer"));
    GtkWidget *btn_delete_customer = GTK_WIDGET(gtk_builder_get_object(builder, "btn_delete_customer"));
    GtkWidget *btn_search_customer = GTK_WIDGET(gtk_builder_get_object(builder, "btn_search_customer"));
    GtkWidget *btn_place_order     = GTK_WIDGET(gtk_builder_get_object(builder, "btn_place_order"));
    GtkWidget *btn_process_order   = GTK_WIDGET(gtk_builder_get_object(builder, "btn_process_order"));
    GtkWidget *btn_undo            = GTK_WIDGET(gtk_builder_get_object(builder, "btn_undo"));
    GtkWidget *btn_view_log        = GTK_WIDGET(gtk_builder_get_object(builder, "btn_view_log"));

    // 4. Connect signals
    g_signal_connect(window,              "destroy", G_CALLBACK(on_main_window_destroy),        NULL);
    g_signal_connect(btn_add,             "clicked", G_CALLBACK(on_btn_add_product_clicked),     NULL);
    g_signal_connect(btn_delete,          "clicked", G_CALLBACK(on_btn_delete_product_clicked),  NULL);
    g_signal_connect(btn_update,          "clicked", G_CALLBACK(on_btn_update_stock_clicked),    NULL);
    g_signal_connect(btn_search,          "clicked", G_CALLBACK(on_btn_search_product_clicked),  NULL);
    g_signal_connect(btn_add_customer,    "clicked", G_CALLBACK(on_btn_add_customer_clicked),    NULL);
    g_signal_connect(btn_delete_customer, "clicked", G_CALLBACK(on_btn_delete_customer_clicked), NULL);
    g_signal_connect(btn_search_customer, "clicked", G_CALLBACK(on_btn_search_customer_clicked), NULL);
    g_signal_connect(btn_place_order,     "clicked", G_CALLBACK(on_btn_place_order_clicked),     NULL);
    g_signal_connect(btn_process_order,   "clicked", G_CALLBACK(on_btn_process_order_clicked),   NULL);
    g_signal_connect(btn_undo,            "clicked", G_CALLBACK(on_btn_undo_clicked),            NULL);
    g_signal_connect(btn_view_log,        "clicked", G_CALLBACK(on_btn_view_log_clicked),        NULL);

    // 5. Load data
    initializeSystem(&app);
    loadSystemData(&app);

    // 6. Status bar
    GtkWidget *parent_box = GTK_WIDGET(gtk_builder_get_object(builder, "main_box"));
    statusbar = gtk_statusbar_new();
    gtk_statusbar_push(GTK_STATUSBAR(statusbar), 0, "  Inventory Management System v1.0  |  Ready");
    gtk_box_pack_end(GTK_BOX(parent_box), statusbar, FALSE, FALSE, 0);

    // 7. Setup TreeViews
    inventory_view = GTK_WIDGET(gtk_builder_get_object(builder, "inventory_tree_view"));
    setup_inventory_treeview(inventory_view);
    inventory_row = 0;
    populate_inventory(app.inventoryRoot);
    update_statusbar();

    customer_view = GTK_WIDGET(gtk_builder_get_object(builder, "customer_tree_view"));
    setup_customer_treeview(customer_view);
    populate_customers();

    order_view = GTK_WIDGET(gtk_builder_get_object(builder, "orders_tree_view"));
    setup_order_treeview(order_view);
    populate_orders();

    admin_view = GTK_WIDGET(gtk_builder_get_object(builder, "admin_tree_view"));
    setup_admin_treeview(admin_view);
    populate_admin_log();

    // 8. Show
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

void on_notebook_switch_page(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer data) {
    char msg[128];
    switch (page_num) {
        case 0: // Inventory
            update_statusbar();
            break;
        case 1: { // Customers
            int count = 0;
            GtkTreeIter iter;
            gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(customer_store), &iter);
            while (valid) { count++; valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(customer_store), &iter); }
            snprintf(msg, sizeof(msg), "  Inventory Management System v1.0  |  %d customers loaded", count);
            gtk_statusbar_pop(GTK_STATUSBAR(statusbar), 0);
            gtk_statusbar_push(GTK_STATUSBAR(statusbar), 0, msg);
            break;
        }
        case 2: { // Orders
            int count = 0;
            GtkTreeIter iter;
            gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(order_store), &iter);
            while (valid) { count++; valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(order_store), &iter); }
            snprintf(msg, sizeof(msg), "  Inventory Management System v1.0  |  %d orders pending", count);
            gtk_statusbar_pop(GTK_STATUSBAR(statusbar), 0);
            gtk_statusbar_push(GTK_STATUSBAR(statusbar), 0, msg);
            break;
        }
        case 3: { // Admin
            int count = 0;
            GtkTreeIter iter;
            gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(admin_store), &iter);
            while (valid) { count++; valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(admin_store), &iter); }
            snprintf(msg, sizeof(msg), "  Inventory Management System v1.0  |  %d log entries", count);
            gtk_statusbar_pop(GTK_STATUSBAR(statusbar), 0);
            gtk_statusbar_push(GTK_STATUSBAR(statusbar), 0, msg);
            break;
        }
    }
}

void update_statusbar() {
    int count = 0;
    GtkTreeIter iter;
    gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(inventory_store), &iter);
    while (valid) {
        count++;
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(inventory_store), &iter);
    }
    char msg[128];
    snprintf(msg, sizeof(msg), "  Inventory Management System v1.0  |  %d products loaded", count);
    gtk_statusbar_pop(GTK_STATUSBAR(statusbar), 0);
    gtk_statusbar_push(GTK_STATUSBAR(statusbar), 0, msg);
}

void on_main_window_destroy(GtkWidget *widget, gpointer data) {
    saveSystemData(&app);
    freeSystem(&app);
    gtk_main_quit();
}

void on_btn_add_product_clicked(GtkButton *button, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Add Product", NULL, GTK_DIALOG_MODAL, "Add", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *lbl_id = gtk_label_new("Product ID:");
    GtkWidget *entry_id = gtk_entry_new();
    GtkWidget *lbl_name = gtk_label_new("Name:");
    GtkWidget *entry_name = gtk_entry_new();
    GtkWidget *lbl_price = gtk_label_new("Price:");
    GtkWidget *entry_price = gtk_entry_new();
    GtkWidget *lbl_stock = gtk_label_new("Stock:");
    GtkWidget *entry_stock = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content), lbl_id);
    gtk_container_add(GTK_CONTAINER(content), entry_id);
    gtk_container_add(GTK_CONTAINER(content), lbl_name);
    gtk_container_add(GTK_CONTAINER(content), entry_name);
    gtk_container_add(GTK_CONTAINER(content), lbl_price);
    gtk_container_add(GTK_CONTAINER(content), entry_price);
    gtk_container_add(GTK_CONTAINER(content), lbl_stock);
    gtk_container_add(GTK_CONTAINER(content), entry_stock);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
    int id = atoi(gtk_entry_get_text(GTK_ENTRY(entry_id)));
    const char *name = gtk_entry_get_text(GTK_ENTRY(entry_name));
    float price = atof(gtk_entry_get_text(GTK_ENTRY(entry_price)));
    int stock = atoi(gtk_entry_get_text(GTK_ENTRY(entry_stock)));

    if (id <= 0 || strlen(name) == 0 || price <= 0 || stock < 0) {
        GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Please fill in all fields correctly!\n- ID must be > 0\n- Name cannot be empty\n- Price must be > 0\n- Stock cannot be negative");
        gtk_dialog_run(GTK_DIALOG(err));
        gtk_widget_destroy(err);
    } else {
        app.inventoryRoot = insertProduct(app.inventoryRoot, id, (char *)name, price, stock);
        gtk_list_store_clear(inventory_store);
        inventory_row = 0;
        populate_inventory(app.inventoryRoot);
        update_statusbar();
    }
        gtk_list_store_clear(inventory_store);
        inventory_row = 0;
        populate_inventory(app.inventoryRoot);
        update_statusbar();
    }
    gtk_widget_destroy(dialog);
}

void on_btn_delete_product_clicked(GtkButton *button, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Delete Product", NULL, GTK_DIALOG_MODAL, "Delete", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *lbl = gtk_label_new("Enter Product ID to delete:");
    GtkWidget *entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content), lbl);
    gtk_container_add(GTK_CONTAINER(content), entry);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int id = atoi(gtk_entry_get_text(GTK_ENTRY(entry)));
        if (id <= 0) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Please enter a valid Product ID!");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
        } else {
            Product *found = searchProduct(app.inventoryRoot, id);
            if (found != NULL) {
                adminDeleteProduct(&app.inventoryRoot, &app.undoStack, id);
                gtk_list_store_clear(inventory_store);
                inventory_row = 0;
                populate_inventory(app.inventoryRoot);
                update_statusbar();
                populate_admin_log();
            } else {
                GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Product ID %d not found!", id);
                gtk_dialog_run(GTK_DIALOG(err));
                gtk_widget_destroy(err);
            }
        }
    }
    gtk_widget_destroy(dialog);
}

void on_btn_update_stock_clicked(GtkButton *button, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Update Stock", NULL, GTK_DIALOG_MODAL, "Update", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *lbl_id = gtk_label_new("Product ID:");
    GtkWidget *entry_id = gtk_entry_new();
    GtkWidget *lbl_stock = gtk_label_new("New Stock:");
    GtkWidget *entry_stock = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content), lbl_id);
    gtk_container_add(GTK_CONTAINER(content), entry_id);
    gtk_container_add(GTK_CONTAINER(content), lbl_stock);
    gtk_container_add(GTK_CONTAINER(content), entry_stock);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int id = atoi(gtk_entry_get_text(GTK_ENTRY(entry_id)));
        int newStock = atoi(gtk_entry_get_text(GTK_ENTRY(entry_stock)));

        if (newStock < 0) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Stock cannot be negative!");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
            gtk_widget_destroy(dialog);
            return;
        }

        if (id <= 0) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Please enter a valid Product ID!");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
        } else {
            Product *product = searchProduct(app.inventoryRoot, id);
            if (product != NULL) {
                adminUpdateStock(app.inventoryRoot, &app.undoStack, id, newStock);
                gtk_list_store_clear(inventory_store);
                inventory_row = 0;
                populate_inventory(app.inventoryRoot);
                update_statusbar();
                populate_admin_log();
            } else {
                GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Product ID %d not found!", id);
                gtk_dialog_run(GTK_DIALOG(err));
                gtk_widget_destroy(err);
            }
        }
    }
    gtk_widget_destroy(dialog);
}

void on_btn_search_product_clicked(GtkButton *button, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Search Product", NULL, GTK_DIALOG_MODAL, "Search", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *lbl = gtk_label_new("Enter Product ID:");
    GtkWidget *entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content), lbl);
    gtk_container_add(GTK_CONTAINER(content), entry);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int id = atoi(gtk_entry_get_text(GTK_ENTRY(entry)));

        if (id <= 0) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Please enter a valid Product ID!");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
        } else {
            Product *product = searchProduct(app.inventoryRoot, id);
            GtkWidget *result;
            if (product != NULL) {
                result = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                    "Found!\nID: %d\nName: %s\nPrice: %.2f\nStock: %d",
                    product->productID, product->name, product->price, product->stock);
            } else {
                result = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Product ID %d not found!", id);
            }
            gtk_dialog_run(GTK_DIALOG(result));
            gtk_widget_destroy(result);
        }
    }
    gtk_widget_destroy(dialog);
}

void on_btn_add_customer_clicked(GtkButton *button, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Add Customer", NULL, GTK_DIALOG_MODAL, "Add", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *lbl_id = gtk_label_new("Customer ID:");
    GtkWidget *entry_id = gtk_entry_new();
    GtkWidget *lbl_name = gtk_label_new("Name:");
    GtkWidget *entry_name = gtk_entry_new();
    GtkWidget *lbl_phone = gtk_label_new("Phone:");
    GtkWidget *entry_phone = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content), lbl_id);
    gtk_container_add(GTK_CONTAINER(content), entry_id);
    gtk_container_add(GTK_CONTAINER(content), lbl_name);
    gtk_container_add(GTK_CONTAINER(content), entry_name);
    gtk_container_add(GTK_CONTAINER(content), lbl_phone);
    gtk_container_add(GTK_CONTAINER(content), entry_phone);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int id = atoi(gtk_entry_get_text(GTK_ENTRY(entry_id)));
        const char *name = gtk_entry_get_text(GTK_ENTRY(entry_name));
        const char *phone = gtk_entry_get_text(GTK_ENTRY(entry_phone));

        if (id <= 0 || strlen(name) == 0 || strlen(phone) == 0) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Please fill in all fields correctly!\n- ID must be > 0\n- Name cannot be empty\n- Phone cannot be empty");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
        } else if (searchCustomer(app.customerTable, id) != NULL) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Customer ID %d already exists!", id);
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
        } else {
            Customer *c = createCustomer(id, (char *)name, (char *)phone);
            insertCustomer(app.customerTable, c);
            populate_customers();
        }
    }
    gtk_widget_destroy(dialog);
}

void on_btn_delete_customer_clicked(GtkButton *button, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Delete Customer", NULL, GTK_DIALOG_MODAL, "Delete", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *lbl = gtk_label_new("Enter Customer ID to delete:");
    GtkWidget *entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content), lbl);
    gtk_container_add(GTK_CONTAINER(content), entry);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int id = atoi(gtk_entry_get_text(GTK_ENTRY(entry)));

        if (id <= 0) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Please enter a valid Customer ID!");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
        } else {
            Customer *found = searchCustomer(app.customerTable, id);
            if (found != NULL) {
                int index = hashFunction(id);
                Customer *current = app.customerTable[index];
                Customer *prev = NULL;
                while (current != NULL && current->CustomerID != id) {
                    prev = current;
                    current = current->next;
                }
                if (prev == NULL) {
                    app.customerTable[index] = current->next;
                } else {
                    prev->next = current->next;
                }
                free(current);
                populate_customers();
            } else {
                GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Customer ID %d not found!", id);
                gtk_dialog_run(GTK_DIALOG(err));
                gtk_widget_destroy(err);
            }
        }
    }
    gtk_widget_destroy(dialog);
}

void on_btn_search_customer_clicked(GtkButton *button, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Search Customer", NULL, GTK_DIALOG_MODAL, "Search", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *lbl = gtk_label_new("Enter Customer ID:");
    GtkWidget *entry = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content), lbl);
    gtk_container_add(GTK_CONTAINER(content), entry);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int id = atoi(gtk_entry_get_text(GTK_ENTRY(entry)));

        if (id <= 0) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Please enter a valid Customer ID!");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
        } else {
            Customer *c = searchCustomer(app.customerTable, id);
            GtkWidget *result;
            if (c != NULL) {
                result = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                    "Found!\nID: %d\nName: %s\nPhone: %s", c->CustomerID, c->Name, c->Phone);
            } else {
                result = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Customer ID %d not found!", id);
            }
            gtk_dialog_run(GTK_DIALOG(result));
            gtk_widget_destroy(result);
        }
    }
    gtk_widget_destroy(dialog);
}

void on_btn_place_order_clicked(GtkButton *button, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Place Order", NULL, GTK_DIALOG_MODAL, "Place", GTK_RESPONSE_OK, "Cancel", GTK_RESPONSE_CANCEL, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *lbl_oid = gtk_label_new("Order ID:");
    GtkWidget *entry_oid = gtk_entry_new();
    GtkWidget *lbl_cid = gtk_label_new("Customer ID:");
    GtkWidget *entry_cid = gtk_entry_new();
    GtkWidget *lbl_pid = gtk_label_new("Product ID:");
    GtkWidget *entry_pid = gtk_entry_new();
    GtkWidget *lbl_qty = gtk_label_new("Quantity:");
    GtkWidget *entry_qty = gtk_entry_new();
    GtkWidget *lbl_ship = gtk_label_new("Shipping Type (standard/express):");
    GtkWidget *entry_ship = gtk_entry_new();
    gtk_container_add(GTK_CONTAINER(content), lbl_oid);
    gtk_container_add(GTK_CONTAINER(content), entry_oid);
    gtk_container_add(GTK_CONTAINER(content), lbl_cid);
    gtk_container_add(GTK_CONTAINER(content), entry_cid);
    gtk_container_add(GTK_CONTAINER(content), lbl_pid);
    gtk_container_add(GTK_CONTAINER(content), entry_pid);
    gtk_container_add(GTK_CONTAINER(content), lbl_qty);
    gtk_container_add(GTK_CONTAINER(content), entry_qty);
    gtk_container_add(GTK_CONTAINER(content), lbl_ship);
    gtk_container_add(GTK_CONTAINER(content), entry_ship);
    gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int oid = atoi(gtk_entry_get_text(GTK_ENTRY(entry_oid)));
        int cid = atoi(gtk_entry_get_text(GTK_ENTRY(entry_cid)));
        int pid = atoi(gtk_entry_get_text(GTK_ENTRY(entry_pid)));
        int qty = atoi(gtk_entry_get_text(GTK_ENTRY(entry_qty)));
        const char *ship = gtk_entry_get_text(GTK_ENTRY(entry_ship));

        if (oid <= 0 || cid <= 0 || pid <= 0 || qty <= 0 || strlen(ship) == 0) {
            GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Please fill in all fields correctly!\n- All IDs must be > 0\n- Quantity must be > 0\n- Shipping type cannot be empty");
            gtk_dialog_run(GTK_DIALOG(err));
            gtk_widget_destroy(err);
        } else {
            int result = validateOrder(app.customerTable, app.inventoryRoot, cid, pid, qty);
            if (result == -1) {
                GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Customer ID %d not found!", cid);
                gtk_dialog_run(GTK_DIALOG(err)); gtk_widget_destroy(err);
            } else if (result == -2) {
                GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Product ID %d not found!", pid);
                gtk_dialog_run(GTK_DIALOG(err)); gtk_widget_destroy(err);
            } else if (result == -3) {
                GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Quantity must be greater than 0!");
                gtk_dialog_run(GTK_DIALOG(err)); gtk_widget_destroy(err);
            } else if (result == -4) {
                Product *p = searchProduct(app.inventoryRoot, pid);
                GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Insufficient stock! Available: %d", p->stock);
                gtk_dialog_run(GTK_DIALOG(err)); gtk_widget_destroy(err);
            } else {
                Order *o = createOrder(oid, cid, pid, qty, ship);
                enqueueOrder(&app.orderFront, &app.orderRear, o);
                populate_orders();
            }
        }
    }
    gtk_widget_destroy(dialog);
}

void on_btn_process_order_clicked(GtkButton *button, gpointer data) {
    if (app.orderFront == NULL) {
        GtkWidget *err = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "No pending orders!");
        gtk_dialog_run(GTK_DIALOG(err));
        gtk_widget_destroy(err);
        return;
    }
    Order *processed = dequeueOrder(&app.orderFront, &app.orderRear);
    GtkWidget *msg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        "Order Processed!\nOrder ID: %d\nCustomer ID: %d\nProduct ID: %d\nQuantity: %d",
        processed->orderID, processed->customerID, processed->productID, processed->quantity);
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
    free(processed);
    populate_orders();
}

void on_btn_undo_clicked(GtkButton *button, gpointer data) {
    if (app.undoStack == NULL) {
        GtkWidget *msg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Nothing to undo!");
        gtk_dialog_run(GTK_DIALOG(msg));
        gtk_widget_destroy(msg);
        return;
    }
    undoLastAction(&app.undoStack, &app.inventoryRoot);
    gtk_list_store_clear(inventory_store);
    inventory_row = 0;
    populate_inventory(app.inventoryRoot);
    update_statusbar();
    populate_admin_log();
    GtkWidget *msg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Last action undone!");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
}

void on_btn_view_log_clicked(GtkButton *button, gpointer data) {
    populate_admin_log();
    GtkWidget *msg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Log refreshed!");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
}