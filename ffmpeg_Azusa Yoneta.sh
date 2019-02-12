//Shorten the videos
ffmpeg -i computerworld.mp4 -ss 00:02 -to 00:46 -c:v copy -c:a copy azusa_computerworld.mp4
ffmpeg -i computerworld.mp3 -ss 00:11 -to 00:51 -c:v copy -c:a copy computerworld4.mp3
ffmpeg -i computerworld.mp3 -ss 00:11 -to 00:51 -c:v copy -c:a copy computerworld4.mp3

ffmpeg -i history.mp4 -ss 19:39 -to 19:50 -c:v copy -c:a copy history1.mp4
ffmpeg -i history.mp4 -ss 04:29 -to 04:36 -c:v copy -c:a copy history2.mp4
ffmpeg -i history.mp4 -ss 40:30 -to 40:37 -c:v copy -c:a copy history3.mp4
ffmpeg -i history.mp4 -ss 43:30 -to 43:37 -c:v copy -c:a copy history4.mp4
ffmpeg -i history.mp4 -ss 44:30 -to 44:37 -c:v copy -c:a copy history5.mp4

ffmpeg -i art1.mp4 -ss 06:13 -to 06:24 -c:v copy -c:a copy back1.mp4
ffmpeg -i art1.mp4 -ss 03:17 -to 03:24 -c:v copy -c:a copy back2.mp4
ffmpeg -i art1.mp4 -ss 05:15 -to 05:22 -c:v copy -c:a copy back3.mp4
ffmpeg -i art1.mp4 -ss 05:34 -to 05:41 -c:v copy -c:a copy back4.mp4
ffmpeg -i art1.mp4 -ss 04:03 -to 04:10 -c:v copy -c:a copy back6.mp4
ffmpeg -i art1.mp4 -ss 01:30 -to 01:38 -c:v copy -c:a copy back5.mp4

//Concatenate 
ffmpeg -i first.mp4 -i history4.mp4 -i history5.mp4 \
  -filter_complex "[0:v] [0:a] [1:v] [1:a] [2:v] [2:a] concat=n=3:v=1:a=1 [v] [a]" \
  -map "[v]" -map "[a]" first2.mp4

ffmpeg -i background1.mp4 -i back4.mp4 -i back2.mp4 \
  -filter_complex "[0:v] [0:a] [1:v] [1:a] [2:v] [2:a] concat=n=3:v=1:a=1 [v] [a]" \
  -map "[v]" -map "[a]" background3.mp4

//Concatenate with txt file
ffmpeg -f concat -safe 0 -i mylist2.txt -c copy concat_output.mov

//Overlayed Two Videos
ffmpeg \
-i computerworld_video.mp4 -i background3.mp4 \
-filter_complex " \
[0:v]setpts=PTS-STARTPTS, scale=480x360[top]; \
[1:v]setpts=PTS-STARTPTS, scale=480x360, \
format=yuva420p, colorchannelmixer=aa=0.5[bottom]; \
[top][bottom]overlay=shortest=1" \
-vcodec libx264 finalvideo2.mp4

//Fade out the audio of my videos
ffmpeg -i azusa_final.mp4 -af afade=t=in:st=0:d=3,afade=t=out:st=35:d=5 azusa_final_fade.mp4

//Code I tried out but didnot use for final video
ffmpeg -i 'space-%03d.png' -c:v libx264 -pix_fmt yuv420p space_q.mp4
ffmpeg -i 'vr-%03d.png' -c:v libx264 -pix_fmt yuv420p vr_q.mp4
ffmpeg  -i vr.mp4 -vf fps=1 vr-%03d.png
ffmpeg -i hands.mp4 -vf vflip -c:a copy hands_flip.mp4 
ffmpeg -i hands.mp4 -vf hflip -c:a copy hands_hflip.mp4 

ffmpeg -i vr_shorter.mp4  \
  -i vr_flip.mp4 \
  -filter_complex "color=white,[0:v:0]pad=iw*2:ih[bg]; [bg][1:v:0]overlay=w" \
  vrgirl.mp4