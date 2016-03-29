using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HW05.ViewModels {
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
            // 加入两个用来测试的item
            this.allItems.Add(new Models.TodoListItem("123", "123", DateTime.Today));
            this.allItems.Add(new Models.TodoListItem("456", "456", DateTime.Now.AddDays(1)));
        }

        public void AddTodoItem(string title, string description, DateTimeOffset date) {
            this.allItems.Add(new Models.TodoListItem(title, description, date));
        }

        public void RemoveTodoItem(Models.TodoListItem item) {
            // DIY
            this.allItems.Remove(item);
            // set selectedItem to null after remove
            this.selectedItem = null;
        }

        public void UpdateTodoItem(Models.TodoListItem item, string title, string description, DateTimeOffset date) {
            // DIY
            item.title = title;
            item.description = description;
            item.date = date;
            // set selectedItem to null after update
            this.selectedItem = null;
        }
    }
}
