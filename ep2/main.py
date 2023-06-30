import numpy as np

from kivy.app import App
from kivy.uix.widget import Widget
from kivy_garden.graph import Graph, LinePlot
from kivy.clock import Clock

from plyer import accelerometer

from kivy.lang import Builder

Builder.load_file("ep2.kv")

class Ep2(Widget):
        def __init__(self, **kwargs):
            super().__init__(**kwargs)
            
            self.numbersA = []
            self.numbersB = []
            self.numbersC = []            

            self.time_list = []
            self.time_counter = 0
            self.time_step = 0.05
            self.max_plot = 5
            self.min_plot = 0
            self.ymax_plot = 15
            self.ymin_plot = -15
            self.graph = None

            self.plotA = None
            self.plotB = None
            self.plotC = None

            self.start_function = None
            
            self.start_plot()

        def start_plot(self):
            self.graph = Graph(xmin = self.min_plot, xmax=self.max_plot,
                                ymin=self.ymin_plot, ymax=self.ymax_plot,
                                border_color = [1,1,1,1])
            self.graph.background_color = 0, 0, 0, 1
            self.ids.box.add_widget(self.graph)

            self.plotA = LinePlot(line_width=1, color=[1,0,0,1])
            self.plotB = LinePlot(line_width=1, color=[0,1,0,1])
            self.plotC = LinePlot(line_width=1, color=[0,0,1,1])

            self.graph.add_plot(self.plotA)
            self.graph.add_plot(self.plotB)
            self.graph.add_plot(self.plotC)
        
        def start(self):
            accelerometer.enable()
            self.ids.start_button.state = 'down'
            self.ids.start_button.disabled = True
            self.start_function = Clock.schedule_interval(self.update_label, self.time_step)

        def stop(self):
            if (self.start_function):
                self.start_function.cancel()
                self.ids.start_button.state = 'normal'
                self.ids.start_button.disabled = False
        
        def update_label(self, *args):  
            val = accelerometer.acceleration[:3]
            if not val == (None, None, None):
                self.numbersA.append(val[0])
                self.numbersB.append(val[1])
                self.numbersC.append(val[2])
                
                self.time_list.append(self.time_counter)
                self.time_counter += self.time_step
            
            if(len(self.numbersA)*self.time_step > self.max_plot):
                self.reset_plot()

            self.plotA.points = [(self.time_list[i], self.numbersA[i]) for i in range(len(self.numbersA))]
            self.plotB.points = [(self.time_list[i], self.numbersB[i]) for i in range(len(self.numbersB))]
            self.plotC.points = [(self.time_list[i], self.numbersC[i]) for i in range(len(self.numbersC))]



        def clear_plot(self):
            self.ids.box.clear_widgets()
            self.start_plot()
            self.reset_plot()
        
        def reset_plot(self):
            self.numbersA = []
            self.numbersB = []
            self.numbersC = []
            self.time_counter = 0
            self.time_list = []
        
        def generate_numbers(self):
            return np.random.uniform(self.ymin_plot, self.ymax_plot)
        
class MainApp(App):
    def build(self):
        return Ep2()
    
if __name__ == '__main__':
    MainApp().run()

        
            

