using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;

namespace HW06.Converters {
    public class CheckedConverter : IValueConverter {

        // Define the Convert method
        public object Convert(object value, Type targetType,
                              object parameter, string language) {
            // The value parameter is the data from the source object.
            return (bool)value;
        }

        // ConvertBack is not implemented for a OneWay binding.
        public object ConvertBack(object value, Type targetType,
                                  object parameter, string language) {
            return (bool)value;
        }
    }
}
