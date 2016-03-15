using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HW03.Models {
    class TodoListItem {
        private string id;

        public string title {
            get; set;
        }

        public string description {
            get; set;
        }

        public bool completed {
            get; set;
        }

        public DateTimeOffset date {
            get; set;
        }

        //日期字段自己写

        public TodoListItem(string title, string description, DateTimeOffset date) {
            this.id = Guid.NewGuid().ToString(); //生成id
            this.title = title;
            this.description = description;
            this.completed = false; //默认为未完成
            this.date = date;
        }
    }
}
