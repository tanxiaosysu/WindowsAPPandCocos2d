using SQLitePCL;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HW06.ViewModels {
    class TodoListItemViewModel {
        private ObservableCollection<Models.TodoListItem> allItems = new ObservableCollection<Models.TodoListItem>();

        public ObservableCollection<Models.TodoListItem> AllItems {
            get {
                return this.allItems;
            }
        }

        private Models.TodoListItem selectedItem = default(Models.TodoListItem);

        public Models.TodoListItem SelectedItem {
            get {
                return selectedItem;
            } set {
                this.selectedItem = value;
            }
        }

        public TodoListItemViewModel() {
            /* 加载数据 */
            string sql = @"SELECT Id, Title, Context, Date
                           FROM TodoTable";
            using (var stmt = App.DBconn.Prepare(sql)) {
                while (SQLiteResult.ROW == stmt.Step()) {
                    this.allItems.Add(new Models.TodoListItem() {
                        id = (long)stmt[0],
                        title = (string)stmt[1],
                        description = (string)stmt[2],
                        date = (string)stmt[3]
                    });
                }
            }
        }

        public void AddTodoItem(string title, string description, DateTimeOffset date) {
            Models.TodoListItem item = new Models.TodoListItem() {
                title = title, description = description, date = date.ToString()
            };
            this.allItems.Add(item);
            /* INSERT */
            try {
                string sql = @"INSERT INTO
                           TodoTable (Title, Context, Date)
                           VALUES(?, ?, ?)";
                using (var stmt = App.DBconn.Prepare(sql)) {
                    stmt.Bind(1, item.title);
                    stmt.Bind(2, item.description);
                    stmt.Bind(3, item.date.ToString());
                    stmt.Step();
                }
            } catch (Exception e) {
                // Todo
            }
        }

        public void RemoveTodoItem(Models.TodoListItem item) {
            // DIY
            this.allItems.Remove(item);
            /* DELETE */
            string sql = @"DELETE FROM
                           TodoTable
                           WHERE ID = ?";
            using (var stmt = App.DBconn.Prepare(sql)) {
                stmt.Bind(1, item.id);
                stmt.Step();
            }
            // set selectedItem to null after remove
            this.selectedItem = null;
        }

        public void UpdateTodoItem(Models.TodoListItem item, string title, string description, DateTimeOffset date) {
            // DIY
            item.title = title;
            item.description = description;
            item.date = date.ToString();
            /* UPDATE */
            string sql = @"UPDATE TodoTable
                           SET Title = ?, Context = ?, Date = ?
                           WHERE ID = ?";
            using (var stmt = App.DBconn.Prepare(sql)) {
                stmt.Bind(1, title);
                stmt.Bind(2, description);
                stmt.Bind(3, date.ToString());
                stmt.Bind(4, item.id);
                stmt.Step();
            }
            // set selectedItem to null after update
            this.selectedItem = null;
        }

        /* QUERY */
        public string QueryTodoItem(string keyword) {
            string result = null;
            string sql = @"SELECT Id, Title, Context, Date
                           FROM TodoTable
                           WHERE ID LIKE ? OR Title LIKE ? OR Context LIKE ? OR Date LIKE ?";
            using (var stmt = App.DBconn.Prepare(sql)) {
                stmt.Bind(1, "%" + keyword + "%");
                stmt.Bind(2, "%" + keyword + "%");
                stmt.Bind(3, "%" + keyword + "%");
                stmt.Bind(4, "%" + keyword + "%");
                while (SQLiteResult.ROW == stmt.Step()) {
                    result += "id: " + ((long)stmt[0]).ToString();
                    result += " title: " + stmt[1];
                    result += " details: " + stmt[2];
                    result += " date: " + stmt[3] + "\n";
                }
            }
            if (result == null) {
                result = "No Record!";
            }
            return result;
        }
    }
}
