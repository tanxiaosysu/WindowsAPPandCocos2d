using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;

namespace HW05.Converters {
    public class VisibilityConverter : IValueConverter {

        // Define the Convert method
        public object Convert(object value, Type targetType,
                              object parameter, string language) {
            // The value parameter is the data from the source object.
            bool? ischeched = value as bool?;
            if (ischeched == true) {
                return Visibility.Visible;
            } else {
                return Visibility.Collapsed;
            }
        }

        // ConvertBack is not implemented for a OneWay binding.
        public object ConvertBack(object value, Type targetType,
                                  object parameter, string language) {
            throw new NotImplementedException();
        }
    }
}
