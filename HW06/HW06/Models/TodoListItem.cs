using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HW06.Models {
    class TodoListItem {
        public long id {
            get; set;
        }

        public string title {
            get; set;
        }

        public string description {
            get; set;
        }

        public bool completed {
            get; set;
        }

        public string date {
            get; set;
        }
    }
}
