/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jawosylu <jawosylu@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/05 11:56:32 by jawosylu          #+#    #+#             */
/*   Updated: 2026/10/05 14:00:04 by jawosylu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

static int heap_cmp(t_heap *heap, int scheduler, int a, int b)
{
  if (scheduler == FIFO)
    return (heap->items[a].seq < heap->items[b].seq);
  else
  {
    if (heap->items[a].deadline != heap->items[b].deadline)
      return (heap->items[a].deadline < heap->items[b].deadline);
    return (heap->items[a].seq < heap->items[b].seq);
  }
}

static void heap_swap(t_heap *heap, int a, int b)
{
  t_request temp;

  temp = heap->items[a];
  heap->items[a] = heap->items[b];
  heap->items[b] = temp;
}

static void heap_sift_down(t_heap *heap, int a, int scheduler)
{
  int i;
  int best;
  int left;

  i = a;
  while ((2 * i) + 1 < heap->size)
  {
    best = i;
    left = (2 * i) + 1;
    if (heap_cmp(heap, scheduler, left, i))
      best = (2 * i) + 1;
    if ((2 * i) + 2 < heap->size && heap_cmp(heap, scheduler, (2 * i) + 2, best))
      best = (2 * i) + 2;
    if (best == i)
      return ;
    heap_swap(heap, i, best);
    i = best;
  }
}

int heap_push(t_heap *heap, int scheduler, t_request request)
{
  int i;
  if (heap->size >= HEAP_CAP)
    return (FAIL);
  heap->items[heap->size] = request;
  heap->size++;
  i = heap->size - 1;
  while (i > 0 && heap_cmp(heap, scheduler, i, (i - 1) / 2))
  {
    heap_swap(heap, i, (i - 1) / 2);
    i = (i - 1) / 2;
  }
  return (SUCESS);
}

int heap_pop(t_heap *heap, int scheduler)
{
  if (heap->size == 0)
    return (FAIL);
  heap->size--;
  heap->items[0] = heap->items[heap->size];
  heap_sift_down(heap, 0, scheduler);
  return (SUCESS);
}
