" Test for timers

source shared.vim

if !has('timers')
  finish
endif

func MyHandler(timer)
  let g:val += 1
endfunc

func MyHandlerWithLists(lists, timer)
  let x = string(a:lists)
endfunc

func Test_oneshot()
  let g:val = 0
  let timer = timer_start(50, 'MyHandler')
  let slept = WaitFor('g:val == 1')
  call assert_equal(1, g:val)
  call assert_inrange(30, 100, slept)
endfunc

func Test_repeat_three()
  let g:val = 0
  let timer = timer_start(50, 'MyHandler', {'repeat': 3})
  let slept = WaitFor('g:val == 3')
  call assert_equal(3, g:val)
  call assert_inrange(80, 200, slept)
endfunc

func Test_repeat_many()
  let g:val = 0
  let timer = timer_start(50, 'MyHandler', {'repeat': -1})
  sleep 200m
  call timer_stop(timer)
  call assert_inrange(2, 4, g:val)
endfunc

func Test_with_partial_callback()
  let g:val = 0
  let s:meow = {}
  function s:meow.bite(...)
    let g:val += 1
  endfunction

  call timer_start(50, s:meow.bite)
  let slept = WaitFor('g:val == 1')
  call assert_equal(1, g:val)
  call assert_inrange(30, 100, slept)
endfunc

func Test_retain_partial()
  call timer_start(50, function('MyHandlerWithLists', [['a']]))
  call test_garbagecollect_now()
  sleep 100m
endfunc

func Test_info()
  let id = timer_start(1000, 'MyHandler')
  let info = timer_info(id)
  call assert_equal(id, info[0]['id'])
  call assert_equal(1000, info[0]['time'])
  call assert_true(info[0]['remaining'] > 500)
  call assert_true(info[0]['remaining'] <= 1000)
  call assert_equal(1, info[0]['repeat'])
  call assert_equal("function('MyHandler')", string(info[0]['callback']))

  let found = 0
  for info in timer_info()
    if info['id'] == id
      let found += 1
    endif
  endfor
  call assert_equal(1, found)

  call timer_stop(id)
  call assert_equal([], timer_info(id))
endfunc

func Test_stopall()
  let id1 = timer_start(1000, 'MyHandler')
  let id2 = timer_start(2000, 'MyHandler')
  let info = timer_info()
  call assert_equal(2, len(info))

  call timer_stopall()
  let info = timer_info()
  call assert_equal(0, len(info))
endfunc

func Test_paused()
  let g:val = 0

  let id = timer_start(50, 'MyHandler')
  let info = timer_info(id)
  call assert_equal(0, info[0]['paused'])

  call timer_pause(id, 1)
  let info = timer_info(id)
  call assert_equal(1, info[0]['paused'])
  sleep 100m
  call assert_equal(0, g:val)

  call timer_pause(id, 0)
  let info = timer_info(id)
  call assert_equal(0, info[0]['paused'])

  let slept = WaitFor('g:val == 1')
  call assert_equal(1, g:val)
  call assert_inrange(0, 10, slept)
endfunc

" vim: ts=2 sw=0 et
