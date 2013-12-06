#include "pebble.h"

float timer_delay=1; /* Создаем переменную для с временем для таймера */
Window *window; /* Создаем указатель на окно */
TextLayer *text_layer;  /* создаем  указатель на текстовый слой */
static BitmapLayer *image_layer; /* создаем  указатель на графический слой */
static GBitmap *image; /* создаем  указатель на изображение в памяти */
bool first_time=true; /* создаем флаг первого запуска */
static const uint32_t images[] = {RESOURCE_ID_DICE_1,RESOURCE_ID_DICE_2,RESOURCE_ID_DICE_3,RESOURCE_ID_DICE_4,RESOURCE_ID_DICE_5,RESOURCE_ID_DICE_6,};


void timer_call() /* эта функция вызывается при срабатываниии таймера */
{
    
    if (first_time == false)/* если запускается не в первый раз... */
    { 
        bitmap_layer_destroy(image_layer);
        gbitmap_destroy(image); /* ...то очищаем память от предыдущей картинки */
    }
    first_time = false; /* сбрасываем флаг первого запуска */
    light_enable_interaction();
    image_layer = bitmap_layer_create(GRect(rand()%(144-75), rand()%(168-75), 75, 75)); 
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(image_layer));
    image = gbitmap_create_with_resource(images[rand() % 6]); /* загружаем в память случайную картинку из подключенных ресурсов */
    bitmap_layer_set_bitmap(image_layer, image); /* выводим загруженную картинку в слой */

    if (timer_delay < 300*1000 ) /* если задержка еще не достигла 300мс... */
    {
        timer_delay=timer_delay/0.7; /* ...увеличиваем задержку... */
        app_timer_register(timer_delay/1000, timer_call, NULL); /* ...и взводим таймер заново */
    }
    else /* если задержка уже больше 300мс... */
    {
        timer_delay=1; /* сбрасываем таймер на начало и выходим - сообщение и картинку мы же уже показали */
        light_enable_interaction();
    }
}

static void handle_tap(AccelAxisType axis, int32_t direction)
{
    if (first_time == true)/* если запускается в первый раз... */
    { 
        text_layer_destroy(text_layer);
    }
    timer_call();
}

int main(void)
{
    window = window_create();  /* Инициализируем окно */
    window_set_background_color(window, GColorBlack); /* устанавливаем фоновый цвет */
    window_set_fullscreen(window, true); /* включаем полноэкранность */
    window_stack_push(window, true);  /* открываем окно */
    
    srand(time(NULL)); /* инициализируем генератор случайных чисел текущем временем */

    text_layer = text_layer_create(GRect(0 , 30, 144, 168)); /* создаем текстовый массив, указываем размер и координаты */
    text_layer_set_text_color(text_layer, GColorWhite);  /* устанавливаем цвет текста */
    text_layer_set_background_color(text_layer, GColorClear);  /* устанавливаем цвет фона */
    text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28)); /* устанавливаем шрифт */
    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter); /* устанавливаем выравнивание по центру */
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));  /* подключаем текстовый слой к основному в качестве дочернего */
    text_layer_set_text(text_layer, "Встряхните часы для броска костей");  /* показываем сообщение при запуске */

    accel_tap_service_subscribe(&handle_tap);
    accel_service_set_sampling_rate(25);

    app_event_loop();  /* ждем событий */

        if (first_time == true)/* если запускается в первый раз... */
    { 
        text_layer_destroy(text_layer);
    }
    else
    {    
        bitmap_layer_destroy(image_layer); /* уничтожаем текстовый слой, освобождаем ресурсы */
        gbitmap_destroy(image); /* уничтожаем массив с графикой, освобождаем ресурсы */
    }

    accel_tap_service_unsubscribe();
    window_destroy(window);  /* уничтожаем главное окно, освобождаем ресурсы */

}
