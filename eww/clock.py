import time
import datetime

if __name__ == "__main__":
    months = ["INVALID", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"]

    while True:
        now = datetime.datetime.now()

        current_year = now.year
        current_month_name = months[now.month]
        current_day = now.day
        current_time_str = f"{now.hour:02d}:{now.minute:02d}"

        time_object_str = f"""
            (box :orientation "vertical" :space-evenly false :spacing 5
                (label :width 100 :text "Welcome PJ")
                (label :width 100 :text "{current_month_name} {current_day}, {current_year}")
                (label :width 100 :text "{current_time_str}")
            )
        """
        time_object_str = ' '.join(time_object_str.split())

        print(time_object_str, flush=True)
        time.sleep(60)
